#include "util.h"
#include "bm2dxi.h"

using namespace bm2dxi;
using namespace std::chrono_literals;

using clock_type = std::chrono::system_clock;

enum play_state
{
    STATE_MENU = 0,
    STATE_DECIDE,
    STATE_PLAY,
    STATE_FAILED,
    STATE_RESULT
};

struct state
{
    play_state stage {};
    game_state* game {};
    std::uint32_t retries {};
    std::chrono::time_point<clock_type> start_time {};
    std::chrono::time_point<clock_type> retry_time {};
} app;

auto inline format_clear_text(const result_frame_player_data* data) -> std::string
{
    switch (data->clear_type_current)
    {
        case 0: return "NO PLAY";
        case 1: return "FAILED";
        case 2: return "ASSIST CLEAR";
        case 3: return "EASY CLEAR";
        case 4: return "CLEAR";
        case 5: return "HARD CLEAR";
        case 6: return "EX HARD CLEAR";
        case 7: return "FULL COMBO CLEAR";
        default:
            return "???";
    }
}

auto inline format_chart_text() -> std::string
{
    auto style = std::string { app.game->play_style == 0 ? "SP": "DP" };

    switch (app.game->difficulty_p1)
    {
        case 0: return style + "B";
        case 1: return style + "N";
        case 2: return style + "H";
        case 3: return style + "A";
        case 4: return style + "L";
        default:
            return "???";
    }
}

auto play_hook_fn(SafetyHookContext&)
{
    if (app.stage == STATE_FAILED)
    {
        auto const duration = std::chrono::duration_cast<std::chrono::duration<float>>
            (clock_type::now() - app.retry_time).count();

        SteamTimeline()->AddTimelineEvent("steam_death", "Retry", std::format("Attempt {}", ++app.retries).c_str(),
            1, -duration, duration, k_ETimelineEventClipPriority_None);
    }

    auto const description = std::format("{} ({})", app.game->music->title_16seg, format_chart_text());

    SteamTimeline()->SetTimelineGameMode(k_ETimelineGameMode_Playing);
    SteamTimeline()->SetTimelineStateDescription(description.c_str(), -3.f);

    app.stage = STATE_PLAY;
    app.start_time = clock_type::now() - 3s;
}

auto failed_hook_fn(SafetyHookContext&)
{
    app.stage = STATE_FAILED;
    app.retry_time = clock_type::now();
}

auto result_hook_fn(SafetyHookContext& ctx)
{
    auto const frame = reinterpret_cast<StageResultDrawFrame*>(ctx.rcx);
    auto const* data = app.game->active_p1 ? &frame->p1: &frame->p2;

    auto icon = "steam_checkmark";

    if (data->clear_type_current == 1)
        icon = "steam_x";

    auto const is_record = data->ex_score_current > data->ex_score_best;

    if (is_record)
        icon = "steam_achievement";

    auto const title = std::format("{} EX", data->ex_score_current);
    auto const description = format_clear_text(data);

    auto const duration = std::chrono::duration_cast<std::chrono::duration<float>>
        (clock_type::now() - app.start_time).count();

    SteamTimeline()->SetTimelineGameMode(k_ETimelineGameMode_Staging);
    SteamTimeline()->AddTimelineEvent(icon, title.c_str(), description.c_str(), 2, -duration, duration + 3.f,
        is_record ? k_ETimelineEventClipPriority_Featured: k_ETimelineEventClipPriority_Standard);

    app.stage = STATE_RESULT;
}

auto exit_hook_fn(SafetyHookContext&)
{
    SteamTimeline()->ClearTimelineStateDescription(0.f);
    SteamTimeline()->SetTimelineGameMode(k_ETimelineGameMode_Menus);

    app.stage = STATE_MENU;
    app.retries = 0;
}

auto bm2dxi::init(std::span<std::uint8_t> base) -> void
{
    auto const game_state_addr = util::find(base, "48 63 15 ? ? ? ? 4C 8D 4C 24");
    if (!game_state_addr) throw std::runtime_error { "Failed to find game state address!" };

    auto const play_hook_addr = util::find(base, "C6 05 ? ? ? ? ? 74");
    if (!play_hook_addr) throw std::runtime_error { "Failed to find play hook address!" };

    auto const failed_hook_addr = util::find(base, "89 3D ? ? ? ? 8B 50");
    if (!failed_hook_addr) throw std::runtime_error { "Failed to find stage fail hook address!" };

    auto const result_hook_addr = util::find(base, "48 8D 0D ? ? ? ? F3 44 0F 10 05");
    if (!result_hook_addr) throw std::runtime_error { "Failed to find result hook address!" };

    auto const exit_hook_addr = util::find(base, "48 8B 89 ? ? ? ? 33 F6");
    if (!exit_hook_addr) throw std::runtime_error { "Failed to find exit hook address!" };

    app.game = reinterpret_cast<game_state*>(util::follow_relative(*game_state_addr, 3, 7));

    auto static play_hook = create_mid(*play_hook_addr, play_hook_fn);
    auto static failed_hook = create_mid(*failed_hook_addr, failed_hook_fn);
    auto static result_hook = create_mid(*result_hook_addr, result_hook_fn);
    auto static exit_hook = create_mid(*exit_hook_addr, exit_hook_fn);
}
