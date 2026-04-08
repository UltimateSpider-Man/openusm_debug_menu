#include "replay_missions.h"

#include "common.h"
#include "func_wrapper.h"

#include "mission_stack_manager.h"

#include "script_object.h"

#include "pause_menu_root.h"

#include "pausemenusystem.h"


#include "comic_panels.h"


#include "game.h"


#include "movie_manager.h"


#include "sound_instance_id.h"

VALIDATE_SIZE(replay_missions, 0x14cu);

replay_missions::replay_missions(FEMenuSystem *a2, int a3, int a4) : FEMenu(a2, 0, a3, a4, 8, 0) {
    THISCALL(0x00614570, this, a2, a3, a4);
}

constexpr auto NUM_MISSIONS = 6u;

const char* missions_list[NUM_MISSIONS] = {
    "start_to_level_s01(debug_menu_entry)",
	"start_to_level_s02(debug_menu_entry)",
	"start_to_level_s03(debug_menu_entry)",
	"start_to_level_s04(debug_menu_entry)",
	"start_to_level_s05(debug_menu_entry)",
	"start_to_level_s06(debug_menu_entry)"
};

void replay_missions::Deactivate(int a2)
{
    mString pack_name{"unlockables_ltd"};

    auto *msm = mission_stack_manager::s_inst;
    if (msm->is_pack_pushed(pack_name)) {
        msm->pop_mission_pack_immediate(pack_name, pack_name);
    }

    this->field_28 &= ~0x80u;
	
	
	THISCALL(0x00614690, this, a2);
}


void replay_missions::scroll(int a1)
{
      THISCALL(0x00614730,this, a1);
}


void replay_missions::scroll_left(int a2)
{
    if (--this->field_110 < 0) {
        this->field_110 = 6;
    }

    this->scroll(1);

static string_hash sfx_id_hash{"fe_ps_udscroll"};
[[maybe_unused]] sound_instance_id id = sub_60B960(sfx_id_hash, 1.0, 1.0);

THISCALL(0x006258B0,this, a2);
}


void replay_missions::scroll_right(int a2)
{
    if (++this->field_110 > 6) {
        this->field_110 = 0;
    }

    this->scroll(-1);

static string_hash sfx_id_hash{"fe_ps_udscroll"};
[[maybe_unused]] sound_instance_id id = sub_60B960(sfx_id_hash, 1.0, 1.0);

THISCALL(0x006259E0,this, a2);
} 

void replay_missions::on_back(int a2)
{
    if (this->field_11C)
    {
        this->field_11C = 0;
        return;
    }

    mString pack_name{"unlockables_ltd"};
    mission_stack_manager::s_inst->pop_mission_pack_immediate(pack_name, pack_name);

    this->field_2C->MakeActive(8);

    comic_panels::game_play_panel()->field_67 = 1;

	
	static string_hash sfx_id_hash{"fe_ul_back"};
[[maybe_unused]] sound_instance_id id = sub_60B960(sfx_id_hash, 1.0, 1.0);

THISCALL(0x00625A90,this, a2);
}


void replay_missions::on_resume_game(int a2)
{
    if (this->field_11C)
    {
        this->field_11C = 0;
        return;
    }

    mString pack_name{"unlockables_ltd"};
    mission_stack_manager::s_inst->pop_mission_pack_immediate(pack_name, pack_name);

    auto *menu_system = this->field_2C;
    if (menu_system->m_index >= 0)
    {
        menu_system->MakeActive(-1);
        g_game_ptr->unpause();
        comic_panels::game_play_panel()->field_67 = this->field_30;
    }
	
	static string_hash sfx_id_hash{"fe_ul_back"};
[[maybe_unused]] sound_instance_id id = sub_60B960(sfx_id_hash, 1.0, 1.0);

THISCALL(0x00625C50,this, a2);
}


 void replay_missions::run_script(const char* func_name)
{;

            string_hash toggle_hash;
            toggle_hash.initialize(0, func_name, 0);
			    auto* v1 = script::gsoi()->parent;
			int function = script::find_function(toggle_hash, v1, 0);
			script::new_thread(function, reinterpret_cast<script_instance*>(script::gsoi()));
    

            script::sub_5028B0(toggle_hash, script::gsoi());
            script::exec_thread(0);
			
			
g_game_ptr->unpause();


        

static string_hash sfx_id_hash{"FE_PS_ACCEPT"};
[[maybe_unused]] sound_instance_id id = sub_60B960(sfx_id_hash, 1.0, 1.0);
}

    


void replay_missions::on_select(int a2)
{
	static string_hash sfx_id_hash{"fe_ps_accept"};
[[maybe_unused]] sound_instance_id id = sub_60B960(sfx_id_hash, 1.0, 1.0);

    //movie_manager::load_and_play_movie(missions_list[this->field_110], nullptr, true);
    //this->run_script("start_to_level_s01(debug_menu_entry)");
    this->field_11C = 1;
    this->field_2C->UpdateButtonPresses();
	
	
	
	THISCALL(0x0062DD40,this, a2);
}

void replay_missions_patch()
{
	
	    {
        FUNC_ADDRESS(address, &replay_missions::on_select);
        //set_vfunc(0x00894AC4, address);
    }

}