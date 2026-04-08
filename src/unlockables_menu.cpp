#include "unlockables_menu.h"

#include "common.h"
#include "func_wrapper.h"
#include "log.h"
#include "utility.h"
#include "trace.h"
#include "panelquad.h"
#include "panelfile.h"


#include "ngl.h"

#include "femenusystem.h"


#include "mission_manager.h"
#include "comic_panels.h"

#include "movie_manager.h"

#include "sound_instance_id.h"

#include "os_developer_options.h"


#include "pause_menu_root.h"



#include "mission_stack_manager.h"

#include "comic_panels.h"
#include "common.h"
#include "entity_base.h"
#include "fe_health_widget.h"
#include "fetext.h"
#include "femanager.h"
#include "femenusystem.h"
#include "igofrontend.h"
#include "mstring.h"
#include "panelquad.h"
#include "pausemenusystem.h"
#include "panelfile.h"
#include "utility.h"
#include "trace.h"
#include "vtbl.h"


#include "game.h"




#include "sound_instance_id.h"

#include "cursor.h"

#include "script_object.h"





VALIDATE_SIZE(unlockables_menu, 0x100u);

unlockables_menu::unlockables_menu(FEMenuSystem *a2, int a3, int a4)
    : FEMenu::FEMenu(a2, 0, a3, a4, 8, 0) {
    THISCALL(0x00614020, this, a2, a3, a4);
}



void unlockables_menu::sub_64BF80(PanelFile *a1, int a2)
{
    TRACE("unlockables_menu::sub_64BF80");

    if constexpr (1)
    {
        auto index_str = mString::from_int(a2);

        auto icon_name = mString{"u_mm_icon_0"} + index_str;
        auto icon_highlight_name = icon_name + "?";
        auto full_icon_name = mString{"u_mm_full_icon_0"} + index_str;
        auto big_icon_name = mString{"u_mm_big_icon_0"} + index_str;

        auto *item = &this->field_38[a2 - 1];

        this->field_38[0]  = a1->GetPQ("icon_name");
        this->field_38[1] = a1->GetPQ("icon_highlight_name");
    }
    else
    {
        THISCALL(0x0064BF80, this, a1, a2);
    }
}


void unlockables_menu::sub_62D6D0(int a2)
{
   THISCALL(0x0062D6D0, this);
}


/*

void unlockables_menu::Init()
{
    TRACE("unlockables_menu::Init");

    if constexpr (1)
    {


        this->field_38[0] = panel_file->GetPQ("u_mm_bkg_01");
        this->field_38[1] = panel_file->GetPQ("u_mm_bkg_02");
        this->field_38[2] = panel_file->GetPQ("u_mm_bkg_03");
        this->field_38[3] = panel_file->GetPQ("u_mm_title_box_01");
        this->field_38[4] = panel_file->GetPQ("u_mm_title_box_02");
        this->field_38[5] = panel_file->GetPQ("u_mm_title_box_03");
        this->field_38[6] = panel_file->GetPQ("u_mm_title_box_04");
        this->field_38[7] = panel_file->GetPQ("u_mm_main_icon");
        this->field_38[8] = panel_file->GetPQ("u_mm_logo_small");
        this->field_38[9] = panel_file->GetPQ("u_mm_title_box_spider");
        this->field_38[10] = panel_file->GetPQ("u_mm_main_icon_detail");
        this->field_38[11] = panel_file->GetPQ("u_mm_main_window_decor");
        this->field_38[12] = panel_file->GetPQ("u_mm_main_window_decor_0");
        this->field_38[13] = panel_file->GetPQ("u_mm_main_window_decor_1");

        this->sub_64BF80(panel_file, 1);
        this->sub_64BF80(panel_file, 2);
        this->sub_64BF80(panel_file, 3);
        this->sub_64BF80(panel_file, 4);
        this->sub_64BF80(panel_file, 5);
        this->sub_64BF80(panel_file, 6);
        this->sub_64BF80(panel_file, 7);

        auto y1 = bit_cast<PanelQuad *>(this->field_84)->GetCenterY();
        auto y2 = bit_cast<PanelQuad *>(this->field_74)->GetCenterY();
        this->field_E4 = static_cast<int>(y1 - y2);

        int num_items = 7;
        if (!os_developer_options::instance->get_flag(mString {"LIMITED_EDITION_DISC"})
            && !mission_manager::s_inst->sub_5C24C0())
        {
            num_items = 6;
        }

        this->field_E8 = this->field_E4 * (num_items - 1);

        int item_count = 7;
        if (!os_developer_options::instance->get_flag(mString {"LIMITED_EDITION_DISC"})
            && !mission_manager::s_inst->sub_5C24C0())
        {
            item_count = 6;
        }
		
		        auto saved_index = this->field_F4;
        this->field_F4 = 0;


        while (this->field_F4 < saved_index)
        {
            ++this->field_F4;
            sub_62D6D0(-1);
        }
    }
    else
    {
        THISCALL(0x00645C20, this);
    }
} */

void unlockables_menu::Init()
{


	

	
	
	    THISCALL(0x00645C20, this);

}

void unlockables_menu::OnDeactivate()
{
	THISCALL(0x00614140, this);
}


#include "mission_stack_manager.h"
#include "resource_manager.h"
#include "resource_partition.h"
#include "game.h"
#include "mstring.h"
#include "trace.h"
#include "osassert.h"

// Launches a single mission .PCPACK by name.
// Example: launch_single_mission_pack("s01_fathers_pride")
//   This will look up "pk_s01_fathers_pride" in the amalgapak,
//   push it onto the mission stack, and flush (blocking load).
void launch_single_mission_pack(const char *mission_name)
{
    TRACE("launch_single_mission_pack", mission_name);

    auto *stack_mgr = mission_stack_manager::s_inst;
    assert(stack_mgr != nullptr);

    // Build the pack name the same way mission_manager::load_script does
    mString pack_name = mString{"pk_"} + mString{mission_name};

    // Verify the pack exists in the amalgapak
    string_hash hash{pack_name.c_str()};
    resource_key key{hash, RESOURCE_KEY_TYPE_PACK};

    if (!resource_manager::get_pack_file_stats(key, nullptr, nullptr, nullptr))
    {
        sp_log("launch_single_mission_pack: pack '%s' not found in amalgapak", pack_name.c_str());
        assert(0 && "PCPACK not found in amalgapak");
        return;
    }

    // Check no other pack load/unload is in progress
    if (stack_mgr->waiting_for_push_or_pop())
    {
        sp_log("launch_single_mission_pack: stack manager busy, cannot push '%s'", pack_name.c_str());
        return;
    }

    // Push the mission pack immediately (blocking):
    //   push_mission_pack_immediate internally calls push_mission_pack
    //   then flushes the streamer with game::render_empty_list
    mString mission_str{mission_name};
    stack_mgr->push_mission_pack_immediate(mission_str, pack_name);

    sp_log("launch_single_mission_pack: loaded '%s'", pack_name.c_str());
}


void unlockables_menu::OnCross(int a2) {
	sp_log("unlockables_menu::OnCross():");

    static bool initialized = false;
    static string_hash fe_ps_accept;
    
    if (!initialized) {
        initialized = true;
    }
    		    static string_hash sound_id ("fe_ps_accept");

    sound_instance_id id = sub_60B960(sound_id, 1.0, 1.0);
    
    int panel_id;
    
    switch (this->field_F4) {
    case 3:
        this->field_2C->MakeActive(10);
        comic_panels::game_play_panel()->field_67 = 1;
		panel_id = 11;
        return;
        
    case 1:
        panel_id = 14;
        break;
        
    case 2:
        panel_id = 12;
        break;
        
    case 0:
        panel_id = 10;
        break;
        
    case 4:
	    panel_id = 13;
        break;
        
    case 5:

        movie_manager::load_and_play_movie("x_men_trailer", "x_men_trailer", true);
        return;
        
    case 6:
        panel_id = 16;
        break;
	case 7:
        panel_id = 17;
        break;        
    default:
        return;
    }
    
    this->field_2C->MakeActive(panel_id);
    comic_panels::game_play_panel()->field_67 = 1;
	THISCALL(0x0062DB20, this, a2);
}

void unlockables_menu::OnTriangle(int a2) {
    sp_log("unlockables_menu::OnTriangle():");

    THISCALL(0x006253C0, this, a2);
}

void unlockables_menu::OnActivate() {
    sp_log("unlockables_menu::OnActivate():");

    THISCALL(0x0062D510, this);
}




void unlockables_menu::_Load()
{
    TRACE("unlockables_menu::Load");

    if constexpr (1)
    {
        for (auto i = 0u; i < 14u; ++i)
        {
            sp_log("%d", i);
            auto *quad = this->field_38[i];
            assert(quad != nullptr);

            quad->TurnOn(true);
        }
    }
    else
    {
        THISCALL(0x00614110, this);
    }
}

void unlockables_menu_patch() {

    {
        FUNC_ADDRESS(address, &unlockables_menu::_Load);
        set_vfunc(0x00894918, address);
    }
	{
        FUNC_ADDRESS(address, &unlockables_menu::Init);
        set_vfunc(0x00894914, address);
    }
	{
        FUNC_ADDRESS(address, &unlockables_menu::OnCross);
        set_vfunc(0x00894954, address);
    }
	{
        FUNC_ADDRESS(address, &unlockables_menu::OnTriangle);
        //set_vfunc(0x00894958, address);
    }

    {
        FUNC_ADDRESS(address, &unlockables_menu::OnActivate);
        set_vfunc(0x00894934, address);
    }
    return;


}
