#include "Nami.h"
#include "../plugin_sdk/plugin_sdk.hpp"
#include "fekapepatu.h"

namespace Nami {
	namespace q_parm {
		float range = 875.f;
		float radius = 100.f;
		float delay = 1;
		float speed = FLT_MAX;
	}
	namespace w_parm {
		float range = 725.f;
		float radius = 0;
		float delay = 0;
		float speed = 0;
	}

	namespace e_parm {
		float range = 800.f;
		float radius = 0;
		float delay = 0;
		float speed = 0;
	}
	namespace r_parm {
		float minrange = 1000.f;
		float range = 2500.f;
		float radius = 290.f;

		float delay = 0.5;
		float speed = 850;
	}

	namespace spell_setting {
		script_spell* q = nullptr;
		script_spell* w = nullptr;
		script_spell* e = nullptr;
		script_spell* r = nullptr;
	}

	namespace draw_settings
	{
		TreeEntry* q = nullptr;
		TreeEntry* w = nullptr;
		TreeEntry* e = nullptr;
		TreeEntry* r = nullptr;
	}

	namespace q_settings {
		TreeEntry* q_use = nullptr;
		TreeEntry* q_use_range = nullptr;
		TreeEntry* q_stun = nullptr;
		TreeEntry* q_slow = nullptr;
		TreeEntry* q_gap = nullptr;

		std::map<game_object_script, TreeEntry*> q_gap_list;
	}

	namespace w_settings {
		TreeEntry* w_use = nullptr;
		TreeEntry* w_mode = nullptr;
		TreeEntry* w_checkmy = nullptr;
		TreeEntry* w_checkmy_hp = nullptr;
		std::map<game_object_script, TreeEntry*> w_ally_list;
		std::map<game_object_script, TreeEntry*> w_ally_slider_list;
		std::map<game_object_script, TreeEntry*> w_ally_hp_list;
		std::map<game_object_script, TreeEntry*> w_enemy_list;
	}

	namespace e_settings {
		TreeEntry* e_use = nullptr;
		std::map<game_object_script, TreeEntry*> e_ally_list;
		std::map<game_object_script, TreeEntry*> e_ally_slider_list;
	}

	namespace r_settings {
		TreeEntry* r_use = nullptr;
		TreeEntry* r_use_slide = nullptr;
		TreeEntry* r_use_range = nullptr;
		TreeEntry* r_gap = nullptr;
	}

	namespace draw_settings {
		TreeEntry* draw_q = nullptr;
		TreeEntry* draw_w = nullptr;
		TreeEntry* draw_e = nullptr;
		TreeEntry* draw_r = nullptr;
		TreeEntry* draw_r_min = nullptr;
		TreeEntry* draw_r_minmap = nullptr;

		TreeEntry* draw_q_color = nullptr;
		TreeEntry* draw_w_color = nullptr;
		TreeEntry* draw_e_color = nullptr;
		TreeEntry* draw_r_color = nullptr;

		TreeEntry* draw_w_mode = nullptr;
	}

	namespace hitchances {
		TreeEntry* hic_q = nullptr;
		TreeEntry* hic_r = nullptr;
	}
	TreeTab* main_tab = nullptr;

	vector jmpDian = vector();

	hit_chance get_hitchance(script_spell* spell)
	{
		if (spell == spell_setting::q) return ((hit_chance)(hitchances::hic_q->get_int() + 3));
		if (spell == spell_setting::r) return ((hit_chance)(hitchances::hic_r->get_int() + 3));
		return hit_chance::medium;
	}

	void use_q() {
		if (!q_settings::q_use->get_bool() && !q_settings::q_slow->get_bool() && !q_settings::q_stun->get_bool())
		{
			return;
		}

		//Q目标选择器
		auto qTarget = target_selector->get_target(q_settings::q_use_range->get_int(), damage_type::magical);

		//减速目标Q
		if (qTarget != nullptr && q_settings::q_slow->get_bool())
		{
			if (spell_setting::q->is_ready())
			{
				//预判
				auto pred = spell_setting::q->get_prediction(qTarget);

				if (pred.hitchance >= get_hitchance(spell_setting::q)) {
					if (qTarget->has_buff_type(buff_type::Slow))
					{
						if (spell_setting::q->cast(pred.get_cast_position()))
						{
							return;
						}
					}
				}
			}
		}

		//连招预判Q
		if (qTarget != nullptr && q_settings::q_use->get_bool() && !q_settings::q_slow->get_bool())
		{
			if (spell_setting::q->is_ready())
			{
				//预判
				auto pred = spell_setting::q->get_prediction(qTarget);
				if (pred.hitchance >= get_hitchance(spell_setting::q) && myhero->get_position().distance(pred.get_cast_position())<= q_settings::q_use_range->get_int()) {
					if (spell_setting::q->cast(pred.get_cast_position()))
					{
						return;
					}
				}
			}
		}
	}

	//我方弹敌方 逻辑
	game_object_script w_select_ally() {
		game_object_script tag = nullptr;
		int lv = -1;

		for (auto&& ally : entitylist->get_ally_heroes())
		{
			//设置的百分比血量
			int set_hp = w_settings::w_ally_hp_list[ally]->get_int();

			//当前血量百分比
			float ally_hp_bfb = ally->get_health_percent();

			if (myhero->get_position().distance(ally) <= w_parm::range && !ally->is_dead() && w_settings::w_ally_list[ally]->get_bool() && set_hp >= ally_hp_bfb)
			{
				for (auto&& enemy : entitylist->get_enemy_heroes())
				{
					if (ally->get_position().distance(enemy->get_position()) <= 650 && !enemy->is_dead())
					{
						if (w_settings::w_ally_slider_list[ally]->get_int() > lv)
						{
							//如果弹的目标是自己 看一下检查自己的开关打开了没
							//弹自己则需要满足  敌人边上有队友 并且能治疗到他y
							if (w_settings::w_checkmy->get_bool() && ally == myhero && myhero->get_health_percent() > w_settings::w_checkmy_hp->get_int())
							{
								for (auto&& ally_t : entitylist->get_ally_heroes())
								{
									if (enemy->get_position().distance(ally_t->get_position()) <= 650 && !enemy->is_dead() && !ally_t->is_dead() && ally_t != myhero)
									{
										lv = w_settings::w_ally_slider_list[ally]->get_int();
										tag = ally;
										break;
									}
								}

								continue;
							}

							lv = w_settings::w_ally_slider_list[ally]->get_int();
							tag = ally;
						}
					}
				}
			}
		}
		return tag;
	}

	//敌方弹我方 逻辑
	game_object_script w_select_ally_enemy() {
		game_object_script tag = nullptr;
		int lv = -1;

		for (auto&& ally : entitylist->get_enemy_heroes())
		{
			if (myhero->get_position().distance(ally) <= w_parm::range && !ally->is_dead() && w_settings::w_enemy_list[ally]->get_bool())
			{
				for (auto&& enemy : entitylist->get_ally_heroes())
				{
					if (ally->get_position().distance(enemy->get_position()) <= 650 && !enemy->is_dead())
					{
						tag = ally;
						return tag;
					}
				}
			}
		}
		return tag;
	}
	void use_w() {
		if (!w_settings::w_use->get_bool())
		{
			return;
		}
		if (!spell_setting::w->is_ready())
		{
			return;
		}

		//我方弹敌方
		if (w_settings::w_mode->get_int() == 0)
		{
			game_object_script obj = w_select_ally();
			if (obj == nullptr)
			{
				return;
			}
			spell_setting::w->cast(obj);
			return;
		}

		//敌方弹我方
		if (w_settings::w_mode->get_int() == 1)
		{
			game_object_script obj = w_select_ally_enemy();
			if (obj == nullptr)
			{
				return;
			}
			spell_setting::w->cast(obj);
			return;
		}
	}

	void use_r() {
		if (!r_settings::r_use->get_bool())
		{
			return;
		}
		if (!spell_setting::r->is_ready())
		{
			return;
		}

		auto rTarget = target_selector->get_target(r_settings::r_use_range->get_int(), damage_type::magical);

		// 参数说明 目标对象  是否AOE     AOE范围（最好设置成技能范围）
		auto pred = spell_setting::r->get_prediction(rTarget, true, r_settings::r_use_range->get_int());

		if (pred.aoe_targets_hit_count() >= r_settings::r_use_slide->get_int())
		{
			spell_setting::r->cast(pred.get_cast_position());
		}
	}

	void combo() {
		use_q();
		use_w();
		use_r();
	}

	void auto_q() {
		if (!q_settings::q_stun->get_bool() && !q_settings::q_gap->get_bool())
		{
			return;
		}
		if (!spell_setting::q->is_ready())
		{
			return;
		}
		auto qTarget = target_selector->get_target(q_parm::range, damage_type::magical);

		if (qTarget->is_valid_target(q_parm::range))
		{
			//判断状态放Q
			if (qTarget->has_buff_type(buff_type::Stun) |//眩晕
				qTarget->has_buff_type(buff_type::Taunt) |//嘲讽
				qTarget->has_buff_type(buff_type::Shred) |
				qTarget->has_buff_type(buff_type::Snare) |//陷阱
				qTarget->has_buff_type(buff_type::Knockup) |//击飞
				qTarget->has_buff_type(buff_type::Asleep) |//睡眠
				qTarget->has_buff_type(buff_type::Charm) |//魅惑
				qTarget->has_buff_type(buff_type::Suppression) |//压制
				qTarget->has_buff_type(buff_type::Polymorph) | //变形
				qTarget->has_buff(buff_hash("CaitlynR")) |
				qTarget->has_buff(buff_hash("NunuR")) |
				qTarget->has_buff(buff_hash("XerathR")) |
				qTarget->has_buff(buff_hash("JannaR")) |
				qTarget->has_buff(buff_hash("VelkozR")) |
				qTarget->has_buff(buff_hash("SionQ")) |
				qTarget->has_buff(buff_hash("JhinW"))

				

				&& q_settings::q_stun->get_bool())
			{
				//预判
				auto pred = spell_setting::q->get_prediction(qTarget);

				if (spell_setting::q->cast(pred.get_cast_position()) && pred.hitchance >= get_hitchance(spell_setting::q))
				{
					return;
				}
			}

			// if (q_settings::q_gap->get_bool ()) { if (qTarget->is_dashing ()) { console->print("ssd");
			//
			// if (spell_setting::q->cast(qTarget->get_position ())) { return; } }
			//
			// }
		}
	}
	//救命W
	void auto_w() {
	
		if (spell_setting::w->is_ready())
		{
			for (auto&& ally : entitylist->get_ally_heroes())
			{

				if (myhero->get_position().distance(ally) <= w_parm::range && ally->get_health_percent() <= 20)
				{
					spell_setting::w->cast(ally);
					return;

				}

			}
		}
	


	}
	void on_update() {
		//人物死亡则不执行
		if (myhero->is_dead())
			return;
		auto_w();
		auto_q();
		if (orbwalker->combo_mode())
		{
			combo();
		}
	}

	//准备施法 可以被打断  部分英雄的防突进在这里
	void on_process_spell_cast(game_object_script sender, spell_instance_script spell)
	{
		//if (sender->is_enemy() && sender->is_ai_hero() && q_settings::q_gap_list[sender]->get_bool())
		if (sender->is_me() && sender->is_ai_hero())
		{
			float enemy_spell_range = 0.0;
			spellslot enemy_spell_slow = spellslot::invalid;

			//EZ的E跳跃位置
			if (sender->get_champion() == champion_id::Ezreal && spell->get_spellslot() == spellslot::e)
			{
				enemy_spell_range = 450;
				vector endpos = spell->get_end_position();
				if (spell->get_start_position().distance(spell->get_end_position()) > enemy_spell_range)
				{
					endpos = spell->get_start_position().extend(spell->get_end_position(), enemy_spell_range);
				}
				jmpDian = endpos;
				if (myhero->get_position().distance(endpos) <= q_parm::range)
				{
					myhero->cast_spell(spellslot::q, endpos);
				}
			}

			//女警Q
			if (sender->get_champion() == champion_id::Caitlyn && spell->get_spellslot() == spellslot::q) {
				if (myhero->get_position().distance(sender->get_position()) <= q_parm::range)
				{
					myhero->cast_spell(spellslot::q, sender->get_position());
				}
			}
			//锤石Q
			if (sender->get_champion() == champion_id::Thresh && spell->get_spellslot() == spellslot::q) {
				if (myhero->get_position().distance(sender->get_position()) <= q_parm::range)
				{
					myhero->cast_spell(spellslot::q, sender->get_position());
				}
			}

			//金克斯W
			if (sender->get_champion() == champion_id::Jinx && spell->get_spellslot() == spellslot::w) {
				if (myhero->get_position().distance(sender->get_position()) <= q_parm::range)
				{
					myhero->cast_spell(spellslot::q, sender->get_position());
				}
			}

			//雪人努努R
			if (sender->get_champion() == champion_id::Nunu && spell->get_spellslot() == spellslot::r) {
				if (myhero->get_position().distance(sender->get_position()) <= q_parm::range)
				{
					myhero->cast_spell(spellslot::q, sender->get_position());
				}
			}
		}

		/*	if (sender->is_me())
			{
				console->print("is me on_process_spell_cast ");

				console->print(spell->get_spell_data()->get_name_cstr());
			}*/
	}

	//新路径回调 部分防突进在这里
	void on_new_path(game_object_script sender, const std::vector<vector>& path, bool is_dash, float dash_speed)
	{
		//通用性防突进
		if (sender->is_ai_hero() && sender->is_enemy())
		{
			if (is_dash && q_settings::q_gap_list[sender]->get_bool())
			{
				console->print("enemy on_new_path ");
				if (myhero->get_position().distance(path[1]) <= q_parm::range)
				{
					myhero->cast_spell(spellslot::q, path[1]);

					//myhero->cast_spell(spellslot::trinket, path[1]);
				}
			}
		}

		if (sender->is_ai_hero() && 1 == 2)
		{
			if (is_dash && sender->is_me())
			{
				console->print("me on_new_path");

				//myhero->cast_spell(spellslot::trinket, path[1]);
			}
		}
	}

	//buff 新增
	void on_buff_gain(game_object_script sender, buff_instance_script buff)
	{
		console->print(buff->get_name_cstr());
	}
	//buff 消失
	void on_buff_lose(game_object_script sender, buff_instance_script buff)
	{
		console->print(buff->get_name_cstr());
	}

	//自动E施法
	void auto_e(game_object_script sender, spell_instance_script spell) {
		if (sender->is_ai_hero() && sender->is_ally())
		{
			if (e_settings::e_use->get_bool() && myhero->get_position().distance(sender)<= e_parm::range)
			{
				if (spell_setting::e->is_ready() && spell->get_last_target_id() > 0 && e_settings::e_ally_list[sender]->get_bool())
				{
					auto tagId = spell->get_last_target_id();

					if (auto target = entitylist->get_object(tagId))
					{
						if (target->is_ai_hero()) {
							//获取攻击目标是不是冠军
							//auto target_id = spell->get_last_target_id();

							if (spell->get_spell_data()->get_name().find("Attack") != std::string::npos )
							{
								spell_setting::e->cast(sender);
								console->print("yes find Attack");
							}
						}
					}
				}
			}
		}
	}

	//技能或者平A  真正释放或者攻击出去的回调
	void on_do_cast(game_object_script sender, spell_instance_script spell)
	{
		auto_e(sender, spell);

		if (sender->is_me() && 1 == 2)
		{
			console->print("------------------------is me on_do_cast ");

			if (sender->get_champion() == champion_id::Zeri && spell->get_spellslot() == spellslot::r)
			{
				//附加娜美E
			}
			if (sender->get_champion() == champion_id::Caitlyn && spell->get_spellslot() == spellslot::r)
			{
				//附加娜美E
			}
			if (sender->get_champion() == champion_id::Draven && spell->get_spellslot() == spellslot::r)
			{
				//附加娜美E
			}
			if (sender->get_champion() == champion_id::Irelia && spell->get_spellslot() == spellslot::r)
			{
				//附加娜美E
			}
			if (sender->get_champion() == champion_id::Xerath && spell->get_spellslot() == spellslot::r)
			{
				//附加娜美E
			}
			if (sender->get_champion() == champion_id::Syndra && spell->get_spellslot() == spellslot::r)
			{
				//附加娜美E
			}
			if (sender->get_champion() == champion_id::Viktor && spell->get_spellslot() == spellslot::r)
			{
				//附加娜美E
			}
			if (sender->get_champion() == champion_id::Veigar && spell->get_spellslot() == spellslot::r)
			{
				//附加娜美E
			}

			if (spell->get_spell_data()->get_name().find("Attack") != std::string::npos)
			{
				console->print("yes find Attack");
			}

			console->print(spell->get_spell_data()->get_name_cstr());
		}
	}

	void on_draw() {
		if (draw_settings::draw_q->get_bool())
		{
			draw_manager->add_circle(myhero->get_position(), q_settings::q_use_range->get_int(), draw_settings::draw_q_color->get_color(), 2);
		}
		if (draw_settings::draw_w->get_bool())
		{
			draw_manager->add_circle(myhero->get_position(), w_parm::range, draw_settings::draw_w_color->get_color(), 2);
		}

		if (draw_settings::draw_e->get_bool())
		{
			draw_manager->add_circle(myhero->get_position(), e_parm::range, draw_settings::draw_e_color->get_color(), 2);
		}

		if (draw_settings::draw_r->get_bool())
		{
			draw_manager->add_circle(myhero->get_position(), r_parm::range, draw_settings::draw_r_color->get_color(), 2);
		}
		if (draw_settings::draw_r_min->get_bool())
		{
			draw_manager->add_circle(myhero->get_position(), r_settings::r_use_range->get_int(), draw_settings::draw_r_color->get_color(), 2);
		}

		if (draw_settings::draw_r_minmap->get_bool())
		{
			draw_manager->draw_circle_on_minimap(myhero->get_position(), r_parm::range, draw_settings::draw_r_color->get_color(), 2);
		}
		if (draw_settings::draw_w_mode->get_bool())
		{
			if (w_settings::w_mode->get_int() == 0)
			{
				vector scrent = myhero->get_hpbar_pos();
				scrent.y = scrent.y + 150;

				draw_manager->add_text_on_screen(scrent, D3DCOLOR_ARGB(255, 255, 255, 255), 20, "W模式:友->敌");
			}
			else {
				vector scrent = myhero->get_hpbar_pos();
				scrent.y = scrent.y + 150;

				draw_manager->add_text_on_screen(scrent, D3DCOLOR_ARGB(255, 255, 255, 255), 20, "W模式:敌->友");
			}
		}

		//	draw_manager->add_circle(jmpDian,100, draw_settings::draw_r_color->get_color(), 2);

			//	draw_manager->add_circle(myhero->get_position(), 1, 1);
				//draw_manager->draw_circle_on_minimap(myhero->get_position(), 1, 1);

			//	draw_manager->add_circle(myhero->get_position(), q_parm::range, D3DCOLOR_ARGB(255, 62, 129, 237));
	}

	void EndlishToChinaese() {
		auto cn = translation->add_language("cn", "CN");

		//Q 翻译
		cn->add_translation({ {translation_hash("Q Settings"), L"Q 配置"},
			{translation_hash("Combo"), L"连招"},
			{translation_hash("Q Use Range"), L"Q 使用范围"},
			{translation_hash("Q Use"), L"Q 使用"},
			{translation_hash("Q Only Target Is Slow Use"), L"Q 仅在目标被减速时使用"},
			{translation_hash("Auto Only Target Can Not Move Use"), L"自动对不可移动目标使用"},
			{translation_hash("Gap"), L"防突进"},
			{translation_hash("Q Gap"), L"防突进Q"},
			{translation_hash("Gap List"), L"防突进列表"} });

		//W 翻译
		cn->add_translation({ {translation_hash("W Settings"), L"W 配置"},
			{translation_hash("W Use"), L"W 使用"},
			{translation_hash("W Mode"), L"W 模式"},
			{translation_hash("Ally->Enemy"), L"友->敌"},
			{translation_hash("Enemy->Ally"), L"敌->友"},
			{translation_hash("If its own Hp% <= X%, the conditions are not considered."), L"自身的 Hp% <= X%，则不考虑这些条件。"},
			{translation_hash("Ally - > Enemy:Ally blacklist"), L"友->敌:友军黑名单(W)"},
			{translation_hash("Enemy -> Ally : Enemy blacklist"), L"敌->友:敌军黑名单(W)"} });

		cn->add_translation({  });

		//E 翻译
		cn->add_translation({ {translation_hash("E Settings"), L"E 配置"} });
		cn->add_translation({ {translation_hash("E Use"), L"E 使用"} });
		cn->add_translation({ {translation_hash("blacklist"), L"黑名单"} });

		//R 翻译
		cn->add_translation({ {translation_hash("R Settings"), L"R 配置"} });
		cn->add_translation({ {translation_hash("R Use"), L"R 使用"} });
		cn->add_translation({ {translation_hash("Enemy >= X  Use"), L"敌人 >= X 使用"} });
		cn->add_translation({ {translation_hash("R Use Range"), L"R 使用范围"} });

		//Draw翻译
		cn->add_translation({ {translation_hash("Draw"), L"线圈"} });
		cn->add_translation({ {translation_hash("Q Draw"), L"Q 线圈"} });
		cn->add_translation({ {translation_hash("W Draw"), L"W 线圈"} });
		cn->add_translation({ {translation_hash("E Draw"), L"E 线圈"} });
		cn->add_translation({ {translation_hash("R Max Range Draw"), L"R 最大范围线圈"} });
		cn->add_translation({ {translation_hash("R Actual Cant Range Draw"), L"R 实际施法范围线圈"} });
		cn->add_translation({ {translation_hash("R Minimap Draw"), L"R 小地图线圈"} });
		cn->add_translation({ {translation_hash("W Mode Draw"), L"W模式文字绘制(人物脚底下)"} });

		cn->add_translation({ {translation_hash("Q Draw Color"), L"Q 线圈颜色"} });
		cn->add_translation({ {translation_hash("W Draw Color"), L"W 线圈颜色"} });
		cn->add_translation({ {translation_hash("E Draw Color"), L"E 线圈颜色"} });
		cn->add_translation({ {translation_hash("R Draw Color"), L"R 线圈颜色"} });
		cn->add_translation({ {translation_hash("Hitchances"), L"命中率"} });

		cn->add_translation({ {translation_hash("W Mode:Ally->Enemy"), L"W模式:友->敌"} });
		cn->add_translation({ {translation_hash("W Mode:Enemy->Ally"), L"W模式:敌->友"} });
	}

	void load() {
		EndlishToChinaese();

		spell_setting::q = plugin_sdk->register_spell(spellslot::q, q_parm::range);
		spell_setting::w = plugin_sdk->register_spell(spellslot::w, w_parm::range);
		spell_setting::e = plugin_sdk->register_spell(spellslot::e, e_parm::range);
		spell_setting::r = plugin_sdk->register_spell(spellslot::r, r_parm::minrange);

		spell_setting::q->set_skillshot(q_parm::delay, q_parm::radius, q_parm::speed, {
		collisionable_objects::yasuo_wall }, skillshot_type::skillshot_circle);
		spell_setting::r->set_skillshot(r_parm::delay, r_parm::radius, r_parm::speed, {
		collisionable_objects::yasuo_wall }, skillshot_type::skillshot_line);

		main_tab = menu->create_tab("carry.Nami", myhero->get_character_data()->get_base_skin_name());
		main_tab->set_assigned_texture(myhero->get_square_icon_portrait());
		//q的列表
		auto q = main_tab->add_tab("q", "Q Settings");
		{
			q->add_separator("q.a1", "Combo");

			q_settings::q_use_range = q->add_slider("q.use.range", "Q Use Range", 800, 1, 875);

			//使用Q选择框
			q_settings::q_use = q->add_checkbox("q.use", "Q Use", true);
			//连招情况下,目标被减速Q
			q_settings::q_slow = q->add_checkbox("q_slow", "Q Only Target Is Slow Use", false);

			q->add_separator("q.a2", "Auto");
			//连招情况下,目标不可移动Q
			q_settings::q_stun = q->add_checkbox("q.stun", "Auto Only Target Can Not Move Use", true);

			//防突进开关
			q->add_separator("q.a3", "Gap");
			q_settings::q_gap = q->add_checkbox("q.gap", "Q Gap", true);
			//防突进列表
			auto gaplist_tab = q->add_tab("q.gaplist", "Gap List"); {
				{
					for (auto&& enemy : entitylist->get_enemy_heroes())
					{
						q_settings::q_gap_list[enemy] = gaplist_tab->add_checkbox("q.gaplist." + enemy->get_base_skin_name(), enemy->get_base_skin_name() + "=" + enemy->get_name(), true);
					}
				}
			}
		}
		//q列表加上q图标
		q->set_assigned_texture(myhero->get_spell(spellslot::q)->get_icon_texture());

		//w的列表

		auto w = main_tab->add_tab("w", "W Settings");
		{
			w_settings::w_use = w->add_checkbox("w.use", "W Use", true);
			w_settings::w_mode = w->add_combobox("w.mode", "W Mode", { { "Ally->Enemy" , nullptr }, {"Enemy->Ally", nullptr } }, 0);
			w_settings::w_checkmy = w->add_checkbox("w.checkmy", "友->敌模式下,W技能目标如果是自己,检查W技能通过敌人目标能否弹射到盟军目标(防止滥用)", false);

			w_settings::w_checkmy->set_tooltip("Ally->Enemy mode: The target of the W skill is yourself, check whether the W skill can bounce to the allied target through the enemy target");
			w_settings::w_checkmy_hp = w->add_slider("w.checkmy.hp", "If its own Hp% <= X%, the conditions are not considered.", 70, 1, 100);

			w->add_separator("q.w1", "Ally - > Enemy:Ally blacklist");
			for (auto&& ally : entitylist->get_ally_heroes())
			{
				w_settings::w_ally_list[ally] = w->add_checkbox("w.ally." + ally->get_base_skin_name(), ally->get_base_skin_name() + "=" + ally->get_name(), true);

				w_settings::w_ally_list[ally]->set_texture(ally->get_square_icon_portrait());
				w_settings::w_ally_slider_list[ally] = w->add_slider("w.ally.lv." + ally->get_base_skin_name(), ally->get_base_skin_name() + "=" + ally->get_name() + " 优先级(Priority Level)", 1, 1, 5);
				int hp = 100;
				//AD默认百分之90血量使用
				if (ally->get_champion() == champion_id::Ezreal |
					ally->get_champion() == champion_id::Ashe |
					ally->get_champion() == champion_id::Draven |
					ally->get_champion() == champion_id::Corki |
					ally->get_champion() == champion_id::Aphelios |
					ally->get_champion() == champion_id::Caitlyn |
					ally->get_champion() == champion_id::Jhin |
					ally->get_champion() == champion_id::Jinx |
					ally->get_champion() == champion_id::Kaisa |
					ally->get_champion() == champion_id::Kalista |
					ally->get_champion() == champion_id::Kindred |
					ally->get_champion() == champion_id::KogMaw |
					ally->get_champion() == champion_id::Lucian |
					ally->get_champion() == champion_id::MissFortune |
					ally->get_champion() == champion_id::Samira |
					ally->get_champion() == champion_id::Sivir |
					ally->get_champion() == champion_id::Tristana |
					ally->get_champion() == champion_id::Twitch |
					ally->get_champion() == champion_id::Varus |
					ally->get_champion() == champion_id::Vayne |
					ally->get_champion() == champion_id::Xayah |
					ally->get_champion() == champion_id::Zeri)
				{
					hp = 100;
				}
				w_settings::w_ally_hp_list[ally] = w->add_slider("w.ally.hp." + ally->get_base_skin_name(), ally->get_base_skin_name() + "=" + ally->get_name() + " Hp%", hp, 1, 100);
				w->add_separator("q.ww."+ ally->get_base_skin_name(), "-------------------");
			
			}

			w->add_separator("q.w2", "Enemy -> Ally : Enemy blacklist");

			for (auto&& enemy : entitylist->get_enemy_heroes())
			{
				w_settings::w_enemy_list[enemy] = w->add_checkbox("w.enemy." + enemy->get_base_skin_name(), enemy->get_base_skin_name() + "=" + enemy->get_name(), true);
				w_settings::w_enemy_list[enemy]->set_texture(enemy->get_square_icon_portrait());
			}

			//w_settings::w_enemytoally = w->add_tab("w.enemytoally", "enemy-ally settings");
		}
		w->set_assigned_texture(myhero->get_spell(spellslot::w)->get_icon_texture());

		auto e = main_tab->add_tab("e", "E Settings");
		{
			e->add_separator("e.e1", "Auto");
			e_settings::e_use = e->add_checkbox("e.use", "E Use", true);
			e->add_separator("e.e2", "blacklist");

			for (auto&& ally : entitylist->get_ally_heroes())
			{
				e_settings::e_ally_list[ally] = e->add_checkbox("e.alla." + ally->get_base_skin_name(), ally->get_base_skin_name() + "=" + ally->get_name(), true);
				e_settings::e_ally_list[ally]->set_texture(ally->get_square_icon_portrait());
			}
		}
		e->set_assigned_texture(myhero->get_spell(spellslot::e)->get_icon_texture());

		auto r = main_tab->add_tab("r", "R Settings");
		{
			r_settings::r_use = r->add_checkbox("r.use", "R Use", true);

			r_settings::r_use_slide = r->add_slider("r.roe", "Enemy >= X  Use", 1, 1, 5);

			r_settings::r_use_range = r->add_slider("r.range", "R Use Range", 1200, 1, 2500);

			//r_settings::r_gap = r->add_checkbox("r.gap", "防突进R",true);
		}
		r->set_assigned_texture(myhero->get_spell(spellslot::r)->get_icon_texture());

		auto draw = main_tab->add_tab("draw", "Draw");
		{
			float color[] = { 0.f, 1.f, 0.f, 1.f };

			draw_settings::draw_q = draw->add_checkbox("draw.q", "Q Draw", true);
			draw_settings::draw_q_color = draw->add_colorpick("draw.q.color", "Q Draw Color", color, true);

			draw_settings::draw_w = draw->add_checkbox("draw.w", "W Draw", true);
			draw_settings::draw_w_color = draw->add_colorpick("draw.w.color", "W Draw Color", color, true);

			draw_settings::draw_e = draw->add_checkbox("draw.e", "E Draw", true);
			draw_settings::draw_e_color = draw->add_colorpick("draw.e.color", "E Draw Color", color, true);

			draw_settings::draw_r = draw->add_checkbox("draw.r", "R Max Range Draw", true);
			draw_settings::draw_r_min = draw->add_checkbox("draw.r_min", "R Actual Cant Range Draw", true);

			draw_settings::draw_r_minmap = draw->add_checkbox("draw.r.minmap", "R Minimap Draw", true);

			draw_settings::draw_r_color = draw->add_colorpick("draw.r.color", "R Draw Color", color, true);

			draw_settings::draw_q->set_texture(myhero->get_spell(spellslot::q)->get_icon_texture());
			draw_settings::draw_w->set_texture(myhero->get_spell(spellslot::w)->get_icon_texture());
			draw_settings::draw_e->set_texture(myhero->get_spell(spellslot::e)->get_icon_texture());
			draw_settings::draw_r->set_texture(myhero->get_spell(spellslot::r)->get_icon_texture());

			draw_settings::draw_w_mode = draw->add_checkbox("draw.w.mode", "W Mode Draw", true);
		}

		auto hic = main_tab->add_tab("hic", "Hitchances");
		{
			hitchances::hic_q = hic->add_combobox("hic.q", "Q Hitchance", { {"Low",nullptr},{"Medium",nullptr },{"High", nullptr},{"Very High",nullptr} }, ((int)(hit_chance::high)-3));

			hitchances::hic_r = hic->add_combobox("hic.r", "R Hitchance", { {"Low",nullptr},{"Medium",nullptr },{"High", nullptr},{"Very High",nullptr} }, ((int)(hit_chance::high)-3));
		}

		//初始化屏幕盒子
		//GetPermashow().Init();
		//GetPermashow().Title = "Objk";
		//GetPermashow().AddElement(draw_settings::testa);

		//---------------------以下是回调方法
		event_handler<events::on_draw>::add_callback(on_draw);

		event_handler<events::on_update>::add_callback(on_update);

		event_handler<events::on_process_spell_cast>::add_callback(on_process_spell_cast);

		event_handler<events::on_new_path>::add_callback(on_new_path);

		// event_handler<events::on_buff_gain>::add_callback(on_buff_gain);

		// event_handler<events::on_buff_lose>::add_callback(on_buff_lose);

		event_handler<events::on_do_cast>::add_callback(on_do_cast);
	}

	void unload() {
		//GetPermashow().Destroy();

		menu->delete_tab(main_tab);

		plugin_sdk->remove_spell(spell_setting::q);
		plugin_sdk->remove_spell(spell_setting::w);
		plugin_sdk->remove_spell(spell_setting::e);
		plugin_sdk->remove_spell(spell_setting::r);

		event_handler<events::on_update>::remove_handler(on_update);
		event_handler<events::on_draw>::remove_handler(on_draw);
		event_handler<events::on_do_cast>::remove_handler(on_do_cast);
		event_handler<events::on_new_path>::remove_handler(on_new_path);
		event_handler<events::on_process_spell_cast>::remove_handler(on_process_spell_cast);
	}
}