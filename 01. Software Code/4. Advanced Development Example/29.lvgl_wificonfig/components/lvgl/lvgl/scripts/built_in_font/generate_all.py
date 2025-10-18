#!/usr/bin/env python3.6

import os

print("Generating 8 px")
os.system("./built_in_font_gen.py --size 8 -o lv_font_montserrat_8.c --bpp 4")
os.system('sed -i \'s|#include "lvgl/lvgl.h"|#include "../../lvgl.h"|\' lv_font_montserrat_8.c')

print("\nGenerating 10 px")
os.system("./built_in_font_gen.py --size 10 -o lv_font_montserrat_10.c --bpp 4")
os.system('sed -i \'s|#include "lvgl/lvgl.h"|#include "../../lvgl.h"|\' lv_font_montserrat_10.c')

print("\nGenerating 12 px")
os.system("./built_in_font_gen.py --size 12 -o lv_font_montserrat_12.c --bpp 4")
os.system('sed -i \'s|#include "lvgl/lvgl.h"|#include "../../lvgl.h"|\' lv_font_montserrat_12.c')

print("\nGenerating 14 px")
os.system("./built_in_font_gen.py --size 14 -o lv_font_montserrat_14.c --bpp 4")
os.system('sed -i \'s|#include "lvgl/lvgl.h"|#include "../../lvgl.h"|\' lv_font_montserrat_14.c')

print("\nGenerating 16 px")
os.system("./built_in_font_gen.py --size 16 -o lv_font_montserrat_16.c --bpp 4")
os.system('sed -i \'s|#include "lvgl/lvgl.h"|#include "../../lvgl.h"|\' lv_font_montserrat_16.c')

print("\nGenerating 18 px")
os.system("./built_in_font_gen.py --size 18 -o lv_font_montserrat_18.c --bpp 4")
os.system('sed -i \'s|#include "lvgl/lvgl.h"|#include "../../lvgl.h"|\' lv_font_montserrat_18.c')

print("\nGenerating 20 px")
os.system("./built_in_font_gen.py --size 20 -o lv_font_montserrat_20.c --bpp 4")
os.system('sed -i \'s|#include "lvgl/lvgl.h"|#include "../../lvgl.h"|\' lv_font_montserrat_20.c')

print("\nGenerating 22 px")
os.system("./built_in_font_gen.py --size 22 -o lv_font_montserrat_22.c --bpp 4")
os.system('sed -i \'s|#include "lvgl/lvgl.h"|#include "../../lvgl.h"|\' lv_font_montserrat_22.c')

print("\nGenerating 24 px")
os.system("./built_in_font_gen.py --size 24 -o lv_font_montserrat_24.c --bpp 4")
os.system('sed -i \'s|#include "lvgl/lvgl.h"|#include "../../lvgl.h"|\' lv_font_montserrat_24.c')

print("\nGenerating 26 px")
os.system("./built_in_font_gen.py --size 26 -o lv_font_montserrat_26.c --bpp 4")
os.system('sed -i \'s|#include "lvgl/lvgl.h"|#include "../../lvgl.h"|\' lv_font_montserrat_26.c')

print("\nGenerating 28 px")
os.system("./built_in_font_gen.py --size 28 -o lv_font_montserrat_28.c --bpp 4")
os.system('sed -i \'s|#include "lvgl/lvgl.h"|#include "../../lvgl.h"|\' lv_font_montserrat_28.c')

print("\nGenerating 30 px")
os.system("./built_in_font_gen.py --size 30 -o lv_font_montserrat_30.c --bpp 4")
os.system('sed -i \'s|#include "lvgl/lvgl.h"|#include "../../lvgl.h"|\' lv_font_montserrat_30.c')

print("\nGenerating 32 px")
os.system("./built_in_font_gen.py --size 32 -o lv_font_montserrat_32.c --bpp 4")
os.system('sed -i \'s|#include "lvgl/lvgl.h"|#include "../../lvgl.h"|\' lv_font_montserrat_32.c')

print("\nGenerating 34 px")
os.system("./built_in_font_gen.py --size 34 -o lv_font_montserrat_34.c --bpp 4")
os.system('sed -i \'s|#include "lvgl/lvgl.h"|#include "../../lvgl.h"|\' lv_font_montserrat_34.c')

print("\nGenerating 36 px")
os.system("./built_in_font_gen.py --size 36 -o lv_font_montserrat_36.c --bpp 4")
os.system('sed -i \'s|#include "lvgl/lvgl.h"|#include "../../lvgl.h"|\' lv_font_montserrat_36.c')

print("\nGenerating 38 px")
os.system("./built_in_font_gen.py --size 38 -o lv_font_montserrat_38.c --bpp 4")
os.system('sed -i \'s|#include "lvgl/lvgl.h"|#include "../../lvgl.h"|\' lv_font_montserrat_38.c')

print("\nGenerating 40 px")
os.system("./built_in_font_gen.py --size 40 -o lv_font_montserrat_40.c --bpp 4")
os.system('sed -i \'s|#include "lvgl/lvgl.h"|#include "../../lvgl.h"|\' lv_font_montserrat_40.c')

print("\nGenerating 42 px")
os.system("./built_in_font_gen.py --size 42 -o lv_font_montserrat_42.c --bpp 4")
os.system('sed -i \'s|#include "lvgl/lvgl.h"|#include "../../lvgl.h"|\' lv_font_montserrat_42.c')

print("\nGenerating 44 px")
os.system("./built_in_font_gen.py --size 44 -o lv_font_montserrat_44.c --bpp 4")
os.system('sed -i \'s|#include "lvgl/lvgl.h"|#include "../../lvgl.h"|\' lv_font_montserrat_44.c')

print("\nGenerating 46 px")
os.system("./built_in_font_gen.py --size 46 -o lv_font_montserrat_46.c --bpp 4")
os.system('sed -i \'s|#include "lvgl/lvgl.h"|#include "../../lvgl.h"|\' lv_font_montserrat_46.c')

print("\nGenerating 48 px")
os.system("./built_in_font_gen.py --size 48 -o lv_font_montserrat_48.c --bpp 4")
os.system('sed -i \'s|#include "lvgl/lvgl.h"|#include "../../lvgl.h"|\' lv_font_montserrat_48.c')

print("\nGenerating 12 px subpx")
os.system("./built_in_font_gen.py --size 12 -o lv_font_montserrat_12_subpx.c --bpp 4 --subpx")
os.system('sed -i \'s|#include "lvgl/lvgl.h"|#include "../../lvgl.h"|\' lv_font_montserrat_12_subpx.c')

print("\nGenerating 28 px compressed")
os.system("./built_in_font_gen.py --size 28 -o lv_font_montserrat_28_compressed.c --bpp 4 --compressed")
os.system('sed -i \'s|#include "lvgl/lvgl.h"|#include "../../lvgl.h"|\' lv_font_montserrat_28_compressed.c')

print("\nGenerating 16 px CJK")
os.system(u"./built_in_font_gen.py --size 16 -o lv_font_simsun_16_cjk.c --bpp 4 --font SimSun.woff -r 0x20-0x7f --symbols Once there is someone I don’t think of, he will say that we will be able to meet her with a lot of power, but at this age, they can each do their own thing or wait, and because they are more talented, they will let them do this again every time, so they only want to learn from Taiwan. The student said that if the society sees a small world, it will be a new one, but it will make the work of all people feel that it is possible to use these things and because time has passed, it is possible that China, the United States, and several systems, governments, many countries, life and university research activities, the world should be in a four-way hope. Under the environment, some children must be highly educated and do not have the ability to use the Internet as a center. Although Japan is still an important place to have a relationship with the market, teachers should point out that other countries’ economic development and education have become very convenient. Then they did not find the same computer and the mentality was unable to work properly. The service will not be better in Taiwan. Is it possible to see how the five domestic ones have nothing to do with it? But when the personnel are compared with the product information data first, in addition to the mainland, please ask for some names in the past few years. If there are also regional technologies, in fact, international participation must not occur, including Taipei, which makes it look like it is often seen. Like to go there is no political words go unit has been it is of course whole process history understand that how opportunity home listen all as long as friends order even with real six ah situation or money method point any experience art you ten main mom increase put forward why in your plan to use things to find between bar design can grow Decided to learn who saw half time to complete with considerable classmates ability others life down the meeting easy to develop people things book facts about self-organizing speech more love to establish related all produce multi-business solution complete words accept informed appointment general promotion process management function hand-drawn water requirements children teach difficult our country tells the content The result of the survey is that the family is established and chooses to run. However, parents write about human beings as for buying, especially cooperation. For example, it has to be discussed according to the work situation, resource reasons, your sports concept, the quality of the software, after such a mental impact, it seems to become part of the participation, so some other parks work hard to study through training and have a common so-called next Behavioral cooperation, cooperative goals, consider long-term opinions, music manufacturers are only affected by everything, or a certain female teacher in the center gets a real way to fast, elementary school, part of the project, women hold sentences, just a paragraph, based on the phenomenon, people land, face attention, news here, continue to believe, policy becomes plan, emphasizes the existence of scholars. Institutional significance represents the curriculum that should not be required in life. Those successful fathers, industry, responsible people, although the direct impact is almost divided into actual collective values, making the category form technology so that seven not only go to their own standards, but also seem to apply or animal phone attitude construction career old so often sit down to organize self-owned specific purpose block conditions. Even if the film science executive committee communicates together for employees to cause natural safety, the president also owns and event design institute language serious story academic film equipment outside the car is basically real long-term set to achieve improvement of the structure of life and change the group support of the hospital is only worth The eighth traffic stage of the scholar is to apply for the supervisor to apply for the same feeling as the TV mother. Hong Kong reporter, pressure, happiness, dare to drink. Maybe people talk about production, fear, physical regulation, positive knowledge, as an institution, but encourage role status experts. It is clear that not only the game plays, the effect of Vietnam, the protection of the Communist Party, the opening of the father's professional funding has been After the work is willing to pay attention to many welcome children hours, the Chinese people show that the Chinese Communist Party out of men to avoid the implementation of vocalism actions can not only the campus interest mountain performance can come back to the director, often no longer electronic subject to the head, finally thank you for the help in addition to the local formal real low sex factors to launch on the price to understand the direction. Responsibility statement industry does a lot of gradually psychological point supply must be simple to use observation often planning to reduce re-business reporting still feel the open field is effective women should be engaged in exerting talents instead administrative bank public media improve the power of the natural community ah the Ministry of Education is more than maintaining parent-child union principals usually lack committee members The results are sometimes far before the teachers control the book. Otherwise, the method is less. Principles have to face. Through suggestions, tools, homework, programs, wisdom, and changes, the same form stands. Think of health as a role in population planning. Just special. Original way. Tradition. Always go to invest and strengthen. Constant pursuit of objects plus than thinking about making Taipei City to get out and join Taiwan. Arrange children middle range elderly both it beijing young end tutorial style women find each other global cost return to the ministry just under the change period non-miss overall adoption is basically called europe is being filled with series early etc quite inadequate total analysis deep report good lies in side laughter so consumer awareness metric nation for In the end, the main people are willing to express more freedom, correctness and richness of the Kuomintang war. How can the Kuomintang war have no choice but to express obvious reforms? I am happy that foreign countries are concerned about the achievements of the Soviet Union. People hear that there is no need to create, regardless of the residents, regardless of the beauty of Iraq, which will bring eternal feelings to all the sons.".encode('utf-8'))
os.system('sed -i \'s|#include "lvgl/lvgl.h"|#include "../../lvgl.h"|\' lv_font_simsun_16_cjk.c')

print("\nGenerating 16 px Hebrew, Persian")
os.system("./built_in_font_gen.py --size 16 -o lv_font_dejavu_16_persian_hebrew.c --bpp 4 --font DejaVuSans.ttf -r 0x20-0x7f,0x5d0-0x5ea,0x600-0x6FF,0xFB50-0xFDFF,0xFE70-0xFEFF")
os.system('sed -i \'s|#include "lvgl/lvgl.h"|#include "../../lvgl.h"|\' lv_font_dejavu_16_persian_hebrew.c')