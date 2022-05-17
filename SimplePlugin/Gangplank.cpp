#include "Gangplank.h"

#include "../plugin_sdk/plugin_sdk.hpp"
#include "fekapepatu.h"

namespace Gangplank {
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

		//if (sender->get_name() == "Lux_Skin05_E_tar_aoe_green")
		//{
		//	//判定处于技能范围
		//	if (sender->get_position().distance(myhero->get_position()) <= 300)
		//	{
		//		console->print("处于技能范围");
		//	}
		//}
	}

	//判断在第一个桶附近已经有没有桶
	game_object_script check_zhouwei_t(game_object_script t, int netword_id) {
		game_object_script t2;
		for (auto&& objec : entitylist->get_attackable_objects()) {
			if (objec->get_name() == "Barrel" & objec->get_network_id() != netword_id) {
				if (t->get_position().distance(objec) <= 360)
				{
					t2 = objec;
					//youle = true;
					break;
				}
			}
		}

		return t2;
	}

	void use_q() {
		if (q_settings::q_use_combo->get_bool())
		{
			if (SpellState(spellslot::q) == spell_state::Ready)
			{
				auto Select_Target = target_selector->get_target(q_parm::range, damage_type::physical);

				/*	if (Select_Target && myhero->get_position().distance(Select_Target)<= q_parm::range)
					{
						spell_setting::q->cast(Select_Target);

						return;
					}*/

					//弹药数
				int ammoCount = spell_setting::e->ammo();

				int tCount = 0;
				for (auto&& enemy_i : entitylist->get_enemy_heroes()) {
					//1500码内有人就开始逻辑
					if (myhero->get_position().distance(enemy_i) <= 1500)
					{
						for (auto&& objec : entitylist->get_attackable_objects()) {
							if (objec->get_name() == "Barrel")
							{
								tCount++;
								if (SpellState(spellslot::q) == spell_state::Ready)
								{
									if (myhero->get_position().distance(objec) <= q_parm::range)
									{
										//如果Q是冷却好的 并且Q范围内有一个桶
										for (auto&& enemy : entitylist->get_enemy_heroes()) {
											if (!enemy->is_dead() && SpellState(spellslot::e) == spell_state::Ready)
											{
												if (enemy->get_position().distance(objec) >= 360)
												{
													//计算桶的距离 不够的话补一个桶

													//获取第二个桶的对象 如果没有则补一个桶
													auto t2 = check_zhouwei_t(objec, objec->get_network_id());
													if (t2)
													{
														//获取到第二个桶的时候 可能需要考虑 第一个桶 还在不在 如果不在了 该怎么办是不是直接敌人脚下放桶进行轰炸
														//在一个是否考虑桶的衰弱时间  防止被A掉？ 这个还有待测试
														//在一个如果敌人已经在Q范围内是搞桶还是直接Q人
														//如果Q冷却了 是否考虑A桶来引爆


													}
													else {
														vector endpos = enemy->get_position();
														if (objec->get_position().distance(endpos) > e_parm::radius)
														{
															endpos = objec->get_position().extend(endpos, e_parm::radius);
														}

														spell_setting::e->cast(endpos);
													}
												}

												//spell_setting::e->cast(enemy->get_position());
												//spell_setting::q->cast(objec);
												return;
											}
										}
									}
								}
								else {
									//Q冷却没好 那么检查平A范围里有没有桶 有的话不在放置桶
									if (myhero->get_position().distance(objec) <= 220)
									{
									}
								}
							}
						}

						if (tCount == 0 && ammoCount >= 2)
						{
							spell_setting::e->cast(myhero->get_position());
						}
					}
				}
			}
		}
	}
	void combo() {
		use_q();
	}

	void auto_attck_Barrel() {
		for (auto&& objec : entitylist->get_attackable_objects()) {
			if (objec->get_name() == "Barrel")
			{
				auto h = objec->get_health();
				if (h == 3 && myhero->get_position().distance(objec) <= 220)
				{
					myhero->issue_order(objec);
					return;
				}
			}
		}
	}
	void on_update() {
		if (myhero->is_dead())
		{
			return;
		}
		auto_attck_Barrel();
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
	void test() {
		auto spellLevelR = spell_setting::r->level();

		console->print("R Level:");
		console->print("%i", spellLevelR);

		//法术穿透
		auto fachuan = myhero->mPercentBonusMagicPenetration();

		console->print("法术穿透");
		console->print("%f", fachuan);

		//自身攻速加成
		auto gongsujiacheng = myhero->mAttackSpeedMod();
		console->print("攻速");
		console->print("%f", gongsujiacheng);

		console->print("攻速 非百分比模式");
		console->print("%f", myhero->mPercentAttackSpeedMod());

		//魔法穿透数值 非百分比
		console->print("魔法穿透");
		console->print("%f", myhero->get_flat_magic_penetration());

		//魔法穿透百分比  得到不是直接的穿透百分比 而是 减去自身穿透的值 比如自身有45%的穿透 那么得到 55% 需要减一下
		console->print("魔法穿透");
		console->print("%f", myhero->get_percent_magic_penetration());

		console->print("法术强度");
		console->print("%f", myhero->get_flat_magic_damage_mod());

		console->print("法术防御");
		console->print("%f", myhero->get_magic_lathality());
		console->print("%f", myhero->get_magical_shield());
		console->print("%f", myhero->get_percent_magic_damage_mod());
		console->print("%f", myhero->get_percent_magic_penetration());
		console->print("%f", myhero->get_percent_magic_reduction());
		console->print("%f", myhero->mPercentBonusMagicPenetration());

		console->print("%f", myhero->get_spell_block());
		//console->print("%f", myhero->spell());
	}

	//计算RQ的伤害值
	void CalcRQSpellDamage() {
		//法穿数值
		auto	magic_fc = myhero->get_flat_magic_penetration();

		//法穿百分比
		auto magic_fc_percent = myhero->get_percent_magic_penetration();

		//法强
		auto fq = myhero->get_flat_magic_damage_mod();

		int  ewDamage = 0;
		switch (spell_setting::r->level())
		{
		case  1:ewDamage = 40;
			break;
		case 2:ewDamage = 100;
			break;
		case 3:ewDamage = 160;
			break;
		case 4:ewDamage = 220;
			break;
		default:
			break;
		}
		//RQ的伤害
		auto fqjc = (fq * 0.3) + ewDamage;

		/*for (auto&& enemy : entitylist->get_enemy_heroes()) {
		}*/
	}

	void load() {
		//EndlishToChinaese();

		spell_setting::q = plugin_sdk->register_spell(spellslot::q, q_parm::range);
		spell_setting::w = plugin_sdk->register_spell(spellslot::w, w_parm::range);
		spell_setting::e = plugin_sdk->register_spell(spellslot::e, e_parm::range);
		spell_setting::r = plugin_sdk->register_spell(spellslot::r, r_parm::range);

		main_tab = menu->create_tab("aio.Gangplank", myhero->get_character_data()->get_base_skin_name());
		main_tab->set_assigned_texture(myhero->get_square_icon_portrait());

		auto q = main_tab->add_tab("q", "Q Settings"); {
			q_settings::q_use_combo = q->add_checkbox("q.use.combo", "Combo Q", true);
		}
		q->set_assigned_texture(myhero->get_spell(spellslot::q)->get_icon_texture());

		auto w = main_tab->add_tab("w", "W Settings"); {
		}
		w->set_assigned_texture(myhero->get_spell(spellslot::w)->get_icon_texture());

		auto e = main_tab->add_tab("e", "E Settings");
		{
			e_settings::e_use_combo = e->add_checkbox("e.use.combo", "Combo E", true);
		}
		e->set_assigned_texture(myhero->get_spell(spellslot::e)->get_icon_texture());

		auto draw = main_tab->add_tab("draw", "Draw");
		{
			float color[] = { 0.f, 1.f, 0.f, 1.f };
			draw_settings::q = draw->add_checkbox("draw.q", "Q Draw", true);
			draw_settings::q_color = draw->add_colorpick("draw.q.color", "Q Draw Color", color, true);

			draw_settings::w = draw->add_checkbox("draw.w", "W Draw", true);
			draw_settings::w_color = draw->add_colorpick("draw.w.color", "W Draw Color", color, true);

			draw_settings::e = draw->add_checkbox("draw.e", "E Draw", true);
			draw_settings::e_color = draw->add_colorpick("draw.e.color", "E Color", color, true);
		}

		//event_handler<events::on_create_object>::add_callback(on_create_object);
		event_handler<events::on_update>::add_callback(on_update);
		event_handler<events::on_draw>::add_callback(on_draw);

		//	test();
	}

	void unload() {
	}
}