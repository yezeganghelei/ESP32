/*
 * @Descripttion :  
 * @version      :  
 * @Author       : Kevincoooool
 * @Date         : 2021-07-05 10:25:59
 * @LastEditors: Please set LastEditors
 * @LastEditTime: 2021-11-02 10:47:31
 * @FilePath     : \esp-idf\pro\KSDIY_ESPCAM\main\page\app_anim.c
 */
#include "app_anim.h"

/**
 * @descripttion: 1
 * @param {lv_obj_t} * Animation object
 * @param {lv_anim_exec_xcb_t} Animation function
 * @param {uint16_t} Animation duration
 * @param {lv_coord_t} Start position
 * @param {lv_coord_t} End position
 * @param {lv_anim_path_cb_t} Animation path
 * @return {*}
 */
void obj_add_anim(lv_obj_t *obj, lv_anim_exec_xcb_t exec_cb, uint16_t time, lv_coord_t start, lv_coord_t end, lv_anim_path_cb_t path_cb)
{

#if (LVGL_VERSION_MAJOR >= 8)
	lv_anim_t a;
	lv_anim_init(&a);
	lv_anim_set_values(&a, start, end);
	lv_anim_set_exec_cb(&a, (lv_anim_exec_xcb_t)exec_cb);
	lv_anim_set_var(&a, obj);
	lv_anim_set_time(&a, time);
	lv_anim_set_playback_time(&a, 0);
	lv_anim_set_path_cb(&a, path_cb);
	// lv_anim_set_ready_cb(&a, spectrum_end_cb);
	lv_anim_start(&a);
#else /* Before v7 */
	lv_anim_t a;
	lv_anim_init(&a);
	lv_anim_set_var(&a, obj);							  // Animation object
	lv_anim_set_exec_cb(&a, (lv_anim_exec_xcb_t)exec_cb); // Animation function
	lv_anim_set_time(&a, time);
	a.start = start;	 // Start value
	a.end = end;		 // End value
	a.path.cb = path_cb; // Animation calculation method
	a.playback_time = 0; // Set playback time to 0 to disable animation playback
	lv_anim_start(&a);	 // Start the animation

#endif
}
