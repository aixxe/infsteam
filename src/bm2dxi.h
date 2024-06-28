#pragma once

namespace bm2dxi
{
	struct music_entry
	{
		/* 0x0000 */ const char title[64];
		/* 0x0040 */ const char title_16seg[64];
		/* 0x0080 */ const char genre[64];
		/* 0x00C0 */ const char artist[64];
		/* 0x0100 */ char pad_0274[752];
	}; static_assert(sizeof(music_entry) == 0x3F0);

	struct game_state
	{
	    /* 0x0000 */ std::int32_t game_type;
	    /* 0x0004 */ std::int32_t play_style;
	    /* 0x0008 */ std::uint8_t pad_0008[4];
	    /* 0x000C */ std::int32_t difficulty_p1;
	    /* 0x0010 */ std::int32_t difficulty_p2;
	    /* 0x0014 */ std::int32_t active_p1;
	    /* 0x0018 */ std::int32_t active_p2;
	    /* 0x001C */ std::uint8_t pad_001C[4];
	    /* 0x0020 */ music_entry* music;
	}; static_assert(sizeof(game_state) == 0x28);

	#pragma pack(push, 1)
	struct result_frame_player_data
	{
		/* 0x0000 */ std::int32_t clear_type_best;
		/* 0x0004 */ std::int32_t clear_type_current;
		/* 0x0008 */ std::int32_t dj_level_best;
		/* 0x000C */ std::int32_t dj_level_current;
		/* 0x0010 */ std::int32_t ex_score_best;
		/* 0x0014 */ std::int32_t ex_score_current;
		/* 0x0018 */ std::int32_t miss_count_best;
		/* 0x001C */ std::int32_t miss_count_current;
		/* 0x0020 */ std::int32_t dj_level_delta_grade_current;
		/* 0x0024 */ std::int32_t dj_level_delta_score_current;
		/* 0x0028 */ std::int8_t show_clear_type_best;
		/* 0x0029 */ std::int8_t show_clear_type_current;
		/* 0x002A */ std::int8_t show_dj_level_best;
		/* 0x002B */ std::int8_t show_dj_level_current;
		/* 0x002C */ std::int8_t show_ex_score_best;
		/* 0x002D */ std::int8_t show_ex_score_current;
		/* 0x002E */ std::int8_t show_miss_count_best;
		/* 0x002F */ std::int8_t show_miss_count_current;
		/* 0x0030 */ std::int32_t highlighted_clear_type;
		/* 0x0034 */ std::int32_t highlighted_dj_level;
		/* 0x0038 */ std::int32_t highlighted_ex_score;
		/* 0x003C */ std::int32_t highlighted_miss_count;
	}; static_assert(sizeof(result_frame_player_data) == 0x40);
	#pragma pack(pop)

	struct StageResultDrawFrame
	{
		/* 0x0000 */ char pad_0000[16];
		/* 0x0010 */ result_frame_player_data p1;
		/* 0x0050 */ result_frame_player_data p2;
	}; static_assert(sizeof(StageResultDrawFrame) == 0x90);

	auto init(std::span<std::uint8_t> base) -> void;
}
