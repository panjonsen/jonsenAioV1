#include "Karma.h"
#include "../plugin_sdk/plugin_sdk.hpp"
#include "fekapepatu.h"

namespace Karma {
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
		TreeEntry* e_mode = nullptr;
		TreeEntry* e_one_yuzhi_checkbox = nullptr;
		TreeEntry* e_one_yuzhi_slide = nullptr;
		TreeEntry* e_one_use_kull = nullptr;
		std::map<game_object_script, TreeEntry*> e_one_ally_list;

		TreeEntry* e_two_yuanxing = nullptr;
		TreeEntry* e_two_attck = nullptr;
		TreeEntry* e_two_yuzhi_checkbox = nullptr;
		TreeEntry* e_two_yuzhi_slide = nullptr;
		TreeEntry* e_two_kill = nullptr;
		std::map<game_object_script, TreeEntry*> e_two_ally_list;
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
		float range = 950.f;
		float radius = 120.f;
		float delay = 0.25;
		float speed = 1700;

		//RQ加强Q的半径
		float RqRange = 280.f;
	}
	namespace w_parm {
		float range = 675.f;
		float radius = 0;
		float delay = 0.25;
		float speed = FLT_MAX;
	}

	namespace e_parm {
		float range = 800.f;
		float radius = 0;
		float delay = 0;
		float speed = FLT_MAX;
	}
	namespace r_parm {
		float range = 0;
		float radius = 0;

		float delay = 0;
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
	spell_state SpellState(spellslot slot) {
		return myhero->get_spell_state(slot);
	}

	void on_create_object(game_object_script sender)
	{
		console->print("-------------");
		console->print(sender->get_name_cstr());

		//Lux_Skin05_E_tar_aoe_green 光辉E技能范围
		//

		//火男W 2个其中一个
		//Brand_Skin04_W_POF_charge
		//Brand_Skin04_W_POF_tar_green

		//泽拉斯
		//Xerath_Skin03_W_aoe_green
		//Xerath_Base_Q_aoe_reticle_green
		//Xerath_Base_R_aoe_reticle_green

		if (sender->get_name() == "Lux_Skin05_E_tar_aoe_green")
		{
			//判定处于技能范围
			if (sender->get_position().distance(myhero->get_position()) <= 300)
			{
				console->print("处于技能范围");
			}
		}
	}

	void e_one_use() {
		if (SpellState(spellslot::e) == spell_state::Ready)
		{
			for (auto&& ally : entitylist->get_ally_heroes())
			{
				//白名单关了 就跳过
				if (!e_settings::e_one_ally_list[ally]->get_bool())
				{
					continue;
				}
				//超出E范围跳过判断
				if (myhero->get_position().distance(ally) > e_parm::range)
				{
					continue;
				}

				if (ally->has_buff(buff_hash("SummonerDot")))
				{
					spell_setting::e->cast(ally);
					return;
				}

				
				for (auto&& buff : ally->get_bufflist())
				{
					if (buff->is_valid() && buff->is_alive())
					{
						if (buff->get_type()==buff_type::Poison)
						{
							spell_setting::e->cast(ally);
							return;
						}
					}
				}
		

				//当前血量
				float hp = ally->get_health();
				//可能收到的伤害
				float pred_hp = health_prediction->get_incoming_damage(ally, 0.2f, true);

				if (e_settings::e_one_use_kull->get_bool())
				{
					if (pred_hp >= hp)
					{
						spell_setting::e->cast(ally);
						return;
					}
				}

				//检查阈值开关
				if (e_settings::e_one_yuzhi_checkbox->get_bool())
				{
					//伤害大于阈值就使用E
					if (pred_hp >= e_settings::e_one_yuzhi_slide->get_int())
					{
						spell_setting::e->cast(ally);
						return;
					}
				}
				else {
					spell_setting::e->cast(ally);
				}
			}
		}
	}

	void use_q() {
		if (q_settings::q_use_combo->get_bool())
		{
			if (SpellState(spellslot::q) == spell_state::Ready)
			{
				//Q目标选择器
				auto qTarget = target_selector->get_target(q_settings::q_use_range->get_int(), damage_type::magical);

				if (myhero->get_position().distance(qTarget) <= q_parm::range)
				{
					auto pred = spell_setting::q->get_prediction(qTarget);
					if (pred.hitchance >= hit_chance::high)
					{
						if (q_settings::q_use_combo_rq->get_bool() && SpellState(spellslot::r) == spell_state::Ready)
						{
							spell_setting::r->cast();
						}
						spell_setting::q->cast(qTarget);
						return;
					}
				}

				//小兵溅射Q逻辑
				if (q_settings::q_use_combo_xiaobing->get_bool())
				{
					for (auto&& enemymin : entitylist->get_enemy_minions())
					{
						if (myhero->get_position().distance(enemymin->get_position()) <= q_settings::q_use_range->get_int())
						{
							for (auto&& enemy : entitylist->get_enemy_heroes())
							{
								//orbwalker->move_to(enemy->get_position());
								if (enemy->get_position().distance(enemymin) <= 200)
								{
									auto predmin = spell_setting::q->get_prediction(enemymin);
									if (predmin.hitchance >= hit_chance::high)
									{
										if (q_settings::q_use_combo_xiaobing_rq->get_bool())
										{
											if (SpellState(spellslot::r) == spell_state::Ready)
											{
												spell_setting::r->cast();
											}
										}
										spell_setting::q->cast(enemymin->get_position());
									}
								}
							}
						}
					}
				}
			}
		}
	}
	void use_has_q() {
		if (q_settings::q_use_has->get_bool())
		{
			if (SpellState(spellslot::q) == spell_state::Ready)
			{
				//Q目标选择器
				auto qTarget = target_selector->get_target(q_settings::q_use_range->get_int(), damage_type::magical);

				if (myhero->get_position().distance(qTarget) <= q_parm::range)
				{
					auto pred = spell_setting::q->get_prediction(qTarget);
					if (pred.hitchance >= hit_chance::high)
					{
						if (q_settings::q_use_has_rq->get_bool() && SpellState(spellslot::r) == spell_state::Ready)
						{
							spell_setting::r->cast();
						}
						spell_setting::q->cast(qTarget);
						return;
					}
				}

				//小兵溅射Q逻辑
				if (q_settings::q_use_has_xiaobing->get_bool())
				{
					for (auto&& enemymin : entitylist->get_enemy_minions())
					{
						if (myhero->get_position().distance(enemymin->get_position()) <= q_settings::q_use_range->get_int())
						{
							for (auto&& enemy : entitylist->get_enemy_heroes())
							{
								//orbwalker->move_to(enemy->get_position());
								if (enemy->get_position().distance(enemymin) <= 200)
								{
									auto predmin = spell_setting::q->get_prediction(enemymin);
									if (predmin.hitchance >= hit_chance::high)
									{
										if (q_settings::q_use_has_xiaobing_rq->get_bool())
										{
											if (SpellState(spellslot::r) == spell_state::Ready)
											{
												spell_setting::r->cast();
											}
										}
										spell_setting::q->cast(enemymin->get_position());
									}
								}
							}
						}
					}
				}
			}
		}
	}

	void use_w() {
		if (w_settings::w_use_combo->get_bool())

		{
			if (SpellState(spellslot::w) == spell_state::Ready)
			{
				//W目标选择器
			//	auto wTarget = target_selector->get_target(w_settings::w_range_slide->get_int(), damage_type::magical);

				for (auto&& enemy : entitylist->get_enemy_heroes()) {
					if (w_settings::w_use_combo->get_bool())
					{
						if (myhero->get_position().distance(enemy) <= w_settings::w_range_slide->get_int())
						{
							if (w_settings::w_use_combo_rw_hp->get_bool())
							{
								if (myhero->get_health_percent() <= w_settings::w_use_combo_rw_hp_slide->get_int())
								{
									spell_setting::r->cast();
								}
								spell_setting::w->cast(enemy);
							}
							else {
								if (w_settings::w_use_combo_rw->get_bool())
								{
									spell_setting::r->cast();
								}
								spell_setting::w->cast(enemy);
								return;
							}
						}
					}
				}
			}
		}
	}

	void auto_w() {
		//for (auto&& enemy : entitylist->get_enemy_heroes()) {
		//	console->print("---------------");
		//	for (auto&& buff : enemy->get_bufflist())
		//	{
		//		if (buff->is_valid()&& buff->is_alive())
		//		{
		//			console->print(buff->get_name_cstr());
		//		}
		//	}
		//}

		if (w_settings::w_use_combo_jinzhan->get_bool() | w_settings::w_auto_stun->get_bool())
		{
			if (SpellState(spellslot::w) == spell_state::Ready) {
				for (auto&& enemy : entitylist->get_enemy_heroes()) {
					if (myhero->get_position().distance(enemy) <= w_settings::w_range_jinzhan_slide->get_int() && w_settings::w_use_combo_jinzhan->get_bool())
					{
						spell_setting::w->cast(enemy);
						return;
					}

					if (enemy->has_buff_type(buff_type::Stun) |
						enemy->has_buff_type(buff_type::Taunt) |
						enemy->has_buff_type(buff_type::Shred) |
						enemy->has_buff_type(buff_type::Snare) |
						enemy->has_buff_type(buff_type::Knockup) |
						enemy->has_buff_type(buff_type::Asleep) |
						enemy->has_buff_type(buff_type::Charm) |
						enemy->has_buff_type(buff_type::Suppression) |
						enemy->has_buff_type(buff_type::Polymorph)
						&& myhero->get_position().distance(enemy) <= w_settings::w_range_slide->get_int() && w_settings::w_auto_stun->get_bool()) {
						spell_setting::w->cast(enemy);
						return;
					}
				}
			}
		}
	}

	void combo() {
		use_q();
		use_w();
	}
	void has() {
		use_has_q();
	}
	void on_update() {
		if (myhero->is_dead())
		{
			return;
		}
		auto_w();
		if (e_settings::e_mode->get_int() == 1)
		{
			e_one_use();
		}
		if (orbwalker->combo_mode())
		{
			combo();
		}
		if (orbwalker->harass())
		{
			has();
		}
		/*	for (auto&& enemy : entitylist->get_enemy_heroes()) {
			}*/
			/*float damage = health_prediction->get_incoming_damage(myhero, 0.2f, true);
			if (damage > 50)
			{
				console->print("收到伤害大于50");
			}*/
	}

	void on_draw() {
		if (draw_settings::q_xiaobing->get_bool())
		{
			for (auto&& enemymin : entitylist->get_enemy_minions()) {
				draw_manager->add_circle(enemymin->get_position(), q_settings::q_use_combo_xiaobing_range->get_int(), draw_settings::q_xiaobing_color->get_color(), 2);
			}
		}
		if (draw_settings::q->get_bool())
		{
			draw_manager->add_circle(myhero->get_position(), q_settings::q_use_range->get_int(), draw_settings::q_color->get_color(), 2);
		}
		if (draw_settings::w->get_bool())
		{
			draw_manager->add_circle(myhero->get_position(), w_settings::w_range_slide->get_int(), draw_settings::w_color->get_color(), 2);
		}
		if (draw_settings::w_jinzhan->get_bool())
		{
			draw_manager->add_circle(myhero->get_position(), w_settings::w_range_jinzhan_slide->get_int(), draw_settings::w_jinzhan_color->get_color(), 2);
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
		EndlishToChinaese();
		spell_setting::q = plugin_sdk->register_spell(spellslot::q, q_parm::range);
		spell_setting::w = plugin_sdk->register_spell(spellslot::w, w_parm::range);
		spell_setting::e = plugin_sdk->register_spell(spellslot::e, e_parm::range);
		spell_setting::r = plugin_sdk->register_spell(spellslot::r, r_parm::range);

		spell_setting::q->set_skillshot(q_parm::delay, q_parm::radius, q_parm::speed, {
		collisionable_objects::yasuo_wall,collisionable_objects::minions }, skillshot_type::skillshot_line);

		main_tab = menu->create_tab("carry.Nami", myhero->get_character_data()->get_base_skin_name());
		main_tab->set_assigned_texture(myhero->get_square_icon_portrait());

		auto q = main_tab->add_tab("q", "Q Settings"); {
			q_settings::q_use_range = q->add_slider("q.use.range", "Q Use Range", q_parm::range, 1, q_parm::range);
			q_settings::q_use_combo_xiaobing_range = q->add_slider("q.use.combo.xiaobing.range", "Splash to enemies through minions Range", 200, 1, 280);
			q->add_separator("q.a1", "Combo");
			q_settings::q_use_combo = q->add_checkbox("q.use.combo", "Combo Q", true);
			q_settings::q_use_combo_rq = q->add_checkbox("q.use.combo.rq", "~Combo Use R", true);
			q_settings::q_use_combo_xiaobing = q->add_checkbox("q.use.combo.xiaobing", "~Splash to enemies through minions", true);
			q_settings::q_use_combo_xiaobing_rq = q->add_checkbox("q.use.combo.xiaobing.rq", "~Splash to enemies through minions Use R", true);

			q->add_separator("q.a2", "Harass");

			q_settings::q_use_has = q->add_checkbox("q.use.has", "Harass Q", true);
			q_settings::q_use_has_rq = q->add_checkbox("q.use.has.rq", "~Harass Use R", true);
			q_settings::q_use_has_xiaobing = q->add_checkbox("q.use.has.xiaobing", "~Splash to enemies through minions", true);
			q_settings::q_use_has_xiaobing_rq = q->add_checkbox("q.use.has.xiaobing.rq", "~Splash to enemies through minions Use R", true);
		}
		q->set_assigned_texture(myhero->get_spell(spellslot::q)->get_icon_texture());

		auto w = main_tab->add_tab("w", "W Settings"); {
			w_settings::w_range_slide = w->add_slider("q.use.range", "W Use Range", w_parm::range, 1, w_parm::range);
			w_settings::w_range_jinzhan_slide = w->add_slider("q.use.range.jinzhan", "W Auto Anti-approach Range", w_parm::range, 1, w_parm::range);

			w_settings::w_use_combo = w->add_checkbox("w.use.combo", "W Combo", true);

			w_settings::w_use_combo_rw = w->add_checkbox("w.use.combo.rq", "~Combo Use R", true);

			w_settings::w_use_combo_rw_hp = w->add_checkbox("w.use.combo.hp", "~Use only when HP <= X%", true);
			w_settings::w_use_combo_rw_hp_slide = w->add_slider("w.use.combo.hp.slide", "HP Settings", 25, 1, 100);

			w_settings::w_auto_stun = w->add_checkbox("w.auto.stun", "W Auto can't move", true);

			w_settings::w_use_combo_jinzhan = w->add_checkbox("w.auwo.jijnzhan", "W Auto Anti-approach", true);
		}
		w->set_assigned_texture(myhero->get_spell(spellslot::w)->get_icon_texture());

		auto e = main_tab->add_tab("e", "E Settings");
		{
			e_settings::e_mode = e->add_combobox("e.mode", "E Mode", { { "No Use" , nullptr },{ "One" , nullptr }, {"Two(Developing)", nullptr } }, 0);

			e->add_separator("e.e1", "Mode One");
			e_settings::e_one_yuzhi_checkbox = e->add_checkbox("e.one.yuzhi", "~Threshold check", true);
			e_settings::e_one_yuzhi_slide = e_settings::e_one_yuzhi_slide = e->add_slider("e.one.yuzhi.slide", "~Damage >= X to use", 100, 1, 1000);
			e_settings::e_one_use_kull = e->add_checkbox("e.one.kill", "may die E", true);

			auto one_block = e->add_tab("e.one.block", "whitelist");
			for (auto&& ally : entitylist->get_ally_heroes())
			{
				e_settings::e_one_ally_list[ally] = one_block->add_checkbox("e.one.block." + ally->get_base_skin_name(), ally->get_base_skin_name() + "=" + ally->get_name(), true);
				e_settings::e_one_ally_list[ally]->set_texture(ally->get_square_icon_portrait());
			}

			e->add_separator("e.e2", "Mode Two(Developing)");
			e_settings::e_two_yuanxing = e->add_checkbox("e.roundskill", "Circular Skill Protection", true);
			e_settings::e_two_kill = e->add_checkbox("e.two.kill", "may die E", true);
			e_settings::e_two_attck = e->add_checkbox("e.attck", "Attck Protect", true);
			e_settings::e_two_yuzhi_checkbox = e->add_checkbox("e.attckthreshold", "~Attck Damage check", true);
			e_settings::e_two_yuzhi_slide = e->add_slider("e.thresholddamage", "~Attck Damage >= X to use", 100, 1, 500);
		}
		e->set_assigned_texture(myhero->get_spell(spellslot::e)->get_icon_texture());

		auto draw = main_tab->add_tab("draw", "Draw");
		{
			float color[] = { 0.f, 1.f, 0.f, 1.f };
			draw_settings::q = draw->add_checkbox("draw.q", "Q Draw", true);
			draw_settings::q_color = draw->add_colorpick("draw.q.color", "Q Draw Color", color, true);

			draw_settings::q_xiaobing = draw->add_checkbox("draw.q.xiaobing", "Q minion splash Draw", true);
			draw_settings::q_xiaobing_color = draw->add_colorpick("draw.q.xiaobing.color", "Q minion splash Color", color, true);

			draw_settings::w = draw->add_checkbox("draw.w", "W Draw", true);
			draw_settings::w_color = draw->add_colorpick("draw.w.color", "W Draw Color", color, true);

			draw_settings::w_jinzhan = draw->add_checkbox("draw.w.jinzhan", "W Anti-approach Draw", true);
			draw_settings::w_jinzhan_color = draw->add_colorpick("draw.w.jinzhan.color", "W Anti-approach Color", color, true);

			draw_settings::e = draw->add_checkbox("draw.e", "E Draw", true);
			draw_settings::e_color = draw->add_colorpick("draw.e.color", "E Color", color, true);
		}

		//event_handler<events::on_create_object>::add_callback(on_create_object);
		event_handler<events::on_update>::add_callback(on_update);
		event_handler<events::on_draw>::add_callback(on_draw);
	}

	void unload() {
	}
}