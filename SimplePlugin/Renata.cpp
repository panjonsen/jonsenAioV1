
#include "Renata.h"
#include "../plugin_sdk/plugin_sdk.hpp"
#include "fekapepatu.h"



namespace Renata {
	namespace q_settings {
		TreeEntry* q_use_combo = nullptr;
		TreeEntry* q_use_combo_rq = nullptr;

		TreeEntry* q_use_combo_xiaobing = nullptr;
		TreeEntry* q_use_combo_xiaobing_rq = nullptr;
		TreeEntry* q_use_combo_xiaobing_range = nullptr;
		TreeEntry* q_use_range = nullptr;

		TreeEntry* q_use_has = nullptr;
		TreeEntry* q_use_has_rq = nullptr;
		TreeEntry* q_use_has_xiaobing = nullptr;
		TreeEntry* q_use_has_xiaobing_rq = nullptr;
	}
	namespace e_settings {
		TreeEntry* e_use_combo = nullptr;
	}

	namespace w_settings {
		TreeEntry* w_use_combo = nullptr;
		TreeEntry* w_use_combo_rw = nullptr;
		TreeEntry* w_use_combo_rw_hp = nullptr;
		TreeEntry* w_use_combo_rw_hp_slide = nullptr;
		TreeEntry* w_auto_stun = nullptr;

		//自动
		TreeEntry* w_use_combo_jinzhan = nullptr;

		TreeEntry* w_range_slide = nullptr;
		TreeEntry* w_range_jinzhan_slide = nullptr;
	}

	namespace q_parm {
		float range = 625.f;
		float radius = 0;
		float delay = 0.25;
		float speed = 2600;
	}
	namespace w_parm {
		float range = 0;
		float radius = 0;
		float delay = 0;
		float speed = FLT_MAX;
	}

	namespace e_parm {
		float range = 1000.f;
		float radius = 360;
		float delay = 0.25f;
		float speed = FLT_MAX;
	}
	namespace r_parm {
		float range = 0;
		float radius = 580.f;
		float radius_min = 170.f;
		float delay = 0.25;
		float speed = FLT_MAX;
	}

	namespace spell_setting {
		script_spell* q = nullptr;
		script_spell* w = nullptr;
		script_spell* e = nullptr;
		script_spell* r = nullptr;
	}

	TreeTab* main_tab = nullptr;

	namespace draw_settings {
		TreeEntry* q = nullptr;
		TreeEntry* q_color = nullptr;
		TreeEntry* q_xiaobing = nullptr;
		TreeEntry* q_xiaobing_color = nullptr;

		TreeEntry* w = nullptr;
		TreeEntry* w_color = nullptr;
		TreeEntry* w_jinzhan = nullptr;
		TreeEntry* w_jinzhan_color = nullptr;

		TreeEntry* e = nullptr;
		TreeEntry* e_color = nullptr;
	}






	void combo() {
		
	}

	void on_update() {
		if (myhero->is_dead())
		{
			return;
		}
		//auto_attck_Barrel();
		if (orbwalker->combo_mode())
		{
			combo();
		}
		if (orbwalker->harass())
		{
			//has();
		}
	}

	void on_draw() {
		if (draw_settings::q->get_bool())
		{
			draw_manager->add_circle(myhero->get_position(), q_parm::range, draw_settings::q_color->get_color(), 2);
		}

		if (draw_settings::e->get_bool())
		{
			draw_manager->add_circle(myhero->get_position(), e_parm::range, draw_settings::e_color->get_color(), 2);
		}
	}

	void EndlishToChinaese() {
		auto cn = translation->add_language("cn", "CN");
		cn->add_translation({
		{translation_hash("Q Settings"), L"Q 配置"},
			{translation_hash("Q Use Range"), L"Q 使用范围"},
			{translation_hash("Splash to enemies through minions Range"), L"小兵溅射范围"},
			{translation_hash("Combo Q"), L"连招 Q"},
			{translation_hash("~Combo Use R"), L"~连招加持 R"},
			{translation_hash("~Splash to enemies through minions"), L"~通过小兵溅射 Q"},
			{translation_hash("~Splash to enemies through minions Use R"), L"~通过小兵溅射 加持R"},
			{translation_hash("W Settings"), L"W 配置"},
			{translation_hash("W Use Range"), L"W 使用范围"},
			{translation_hash("W Auto Anti-approach Range"), L"W 自动防近战靠近范围"},
			{translation_hash("W Combo"), L"连招 W"},
			{translation_hash("~Combo Use R"), L"~连招加持R"},
			{translation_hash("~Use only when HP <= X%"), L"~仅当血量 <= X% 时 加持R"},
			{translation_hash("HP Settings"), L"血量设置百分比进度条"},
			{translation_hash("W Auto can't move"), L"自动W无法移动目标"},
			{translation_hash("W Auto Anti-approach"), L"自动防近战靠近 W"},
			{translation_hash("E Settings"), L"E配置"},
			{translation_hash("E Mode"), L"E 模式"},
			{translation_hash("No Use"), L"不使用"},
			{translation_hash("One"), L"保护模式一"},
			{translation_hash("~Threshold check"), L"伤害阈值检查"},
			{translation_hash("~Damage >= X to use"), L"伤害 >= X 才使用"},
			{translation_hash("may die E"), L"可能会死使用E"},
			{translation_hash("Mode Two(Developing)"), L"保护模式二(正在开发中)"},
			{translation_hash("Circular Skill Protection"), L"圆形技能保护"},
			{translation_hash("Attck Protect"), L"平A保护"},
			{translation_hash("~Attck Damage check"), L"平A阈值检查"},
			{translation_hash("~Attck Damage >= X to use"), L"平A伤害 >= X 才使用"},
			{translation_hash("Q Draw"), L"Q 线圈"},
			{translation_hash("Q Draw Color"), L"Q 线圈颜色"},
			{translation_hash("Q minion splash Draw"), L"Q 小兵溅射范围线圈"},
			{translation_hash("Q minion splash Color"), L"Q 小兵溅射范围线圈颜色"},
			{translation_hash("W Draw"), L"W 线圈"},
			{translation_hash("W Anti-approach Draw"), L"W 防近战靠近线圈"},
			{translation_hash("W Anti-approach Color"), L"W 防近战靠近线圈颜色"},
			{translation_hash("E Draw"), L"E 线圈"},
			{translation_hash("E Color"), L"E线圈颜色"},
			{translation_hash("Two(Developing)"), L"保护模式二(开发中)"},
			{translation_hash("whitelist"), L"白名单"},
			{translation_hash("Harass Q"), L"骚扰 Q"},
			{translation_hash("~Harass Use R"), L"~骚扰加持 R"}
			});
	}




	void load() {
		//EndlishToChinaese();

		spell_setting::q = plugin_sdk->register_spell(spellslot::q, q_parm::range);
		spell_setting::w = plugin_sdk->register_spell(spellslot::w, w_parm::range);
		spell_setting::e = plugin_sdk->register_spell(spellslot::e, e_parm::range);
		spell_setting::r = plugin_sdk->register_spell(spellslot::r, r_parm::range);

		main_tab = menu->create_tab("aio.Gangplank", myhero->get_character_data()->get_base_skin_name());
		main_tab->set_assigned_texture(myhero->get_square_icon_portrait());

		
	}

	void unload() {
	}
}