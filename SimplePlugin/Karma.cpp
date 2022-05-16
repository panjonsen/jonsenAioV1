#include "Karma.h"
#include "../plugin_sdk/plugin_sdk.hpp"
#include "fekapepatu.h"

namespace Karma {

	namespace e_settings {
		TreeEntry* e_use = nullptr;
	
	}

	TreeTab* main_tab = nullptr;
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


	void on_update() {

	/*	for (auto&& enemy : entitylist->get_enemy_heroes()) {
		
		
		}*/
		float damage = health_prediction->get_incoming_damage(myhero, 0.2f, true);
		if (damage > 50)
		{
		
			console->print("收到伤害大于50");
		}
		
	
	}


	namespace e_settings{
		TreeEntry* e_mode = nullptr;
		TreeEntry* e_one_yuzhi_checkbox = nullptr;
		TreeEntry* e_one_yuzhi_slide = nullptr;
		TreeEntry* e_one_use_kull = nullptr;



	
	}
	
	void load() {




		main_tab = menu->create_tab("carry.Nami", myhero->get_character_data()->get_base_skin_name());
		main_tab->set_assigned_texture(myhero->get_square_icon_portrait());



		auto e = main_tab->add_tab("e", "E Settings"); 
		{
		 e_settings::e_mode=	e->add_combobox("e.mode", "E 模式", { { "不使用" , nullptr },{ "模式一" , nullptr }, {"模式二", nullptr } }, 0);


		e->add_separator("e.e1", "保护模式一(可能受到伤害使用E)");

		e_settings::e_one_yuzhi_checkbox= e->add_checkbox("e.one.yuzhi", "~E伤害阈值检查开关", true);
	
			e_settings::e_one_yuzhi_slide= e->add_slider("e.one.yuzhi.slide", "~伤害大于 X 则使用E", 100,1,1000);
		
			e_settings::e_one_use_kull= e->add_checkbox("e.one.kill", "可能会死使用E", true);


			e->add_separator("e.e2", "保护模式二(圆形技能和平A伤害以及平A对象)");
			//圆形技能
			 e->add_checkbox("e.roundskill", "圆形技能保护", true);
			 e->add_checkbox("e.attck", "平A保护", true);
			 e->add_checkbox("e.attckthreshold", "平A阈值检查开关", true);
			 e->add_slider("e.thresholddamage", "~平A伤害大于 X 则使用E", 100, 1, 500);
		}


		//event_handler<events::on_create_object>::add_callback(on_create_object);
		event_handler<events::on_update>::add_callback(on_update);
	}

	void unload() {
	}
}