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
os.system(u"./built_in_font_gen.py --size 16 -o lv_font_simsun_16_cjk.c --bpp 4 --font SimSun.woff -r 0x20-0x7f --symbols （），Stealing the Yang Band Nose Painting Lightlyッbookェルwrite about fatherぁフThe fourth thoughtOFuyuantingchang Tianxu BirdれTalking about the ape's painstaking steps gave Zhishou8I wish you a happy lifeぺIt's still hot, the taste is hot, the dormitory is accurate, and the word is safe.ァIf there is a problem, it should be inspectedねAdd daytime ingredients to the Internet to avoid the evil autumnコThe state of the product belongs to the long-awaited special waiting road. I wish that the needle is indeed afraid of sorrow and wind, but of happiness again.っattachedぷnow that4The price of the mother-in-law is frozen, and the only way to sail across the ocean is to the right.よSpecialized in acid treatment, the ratio of Xiangjian Shengfeng's writing experience is suitable for repairing. According to details, he is busy with the supervisor's order.ザFlowers bloom more and moreミYutongtong is short of oilラ。campスHuitiao Nongjingshujian Aijianbao ZhibaヤBefore I heard about the poor bridgeジJian Dang LinネSend Xi Dao a supplementィAwakening the body and traveling the universeョAwesome wordsくつAll the time, the people are chasing after the strange thingsそWe only have chest weapons to defeat Quan Shen LiguangんShenggou Category II North Community Value Test9He WuliゃTrade lessons one by oneゲてcollar3Encourage the establishment of commentaries１The details of the time and space are as follows:おRong Gui takes advantage of his wife's country's concerns and is anxious to take advantage of the quiet time, so he must discuss and control the soilオEven if the lotus is more meat, it is still mixed with the ancient ferry.がIs it true that the ancient doctor Yi Lintian Xingwan took it?60The traveling woman measured her painテsunうEnvironmentally Friendly and PlayfulぼちSitting on one shoulder and waistタょHope the fruit will come trueぶthings to wait forみGao JiuzhaoやヶAllグgo」サ、Recording of Qi Qi's wine and Xu Zhongqi's laughterリsilver cutギQuick question, Manchu, Huang Jisen, Mishiken, Xisuji, Qianzhou Kawajo, MomノJust two words, even though the matchmaker is hereづBut something strange happenedりComrade Zhidao has viewed this questionぞをHengkaiyindi seat fee holders how to choose Yuan withdrawal limitーDrinking at the competition will make you uselessいガmanyケNo Yiyuange Neighbor Qian Mouxue analyzes and picks it upきside membersゼwhite wedding movieへXianya Shijun's painting is like a suspended grid car.わShopkeeper Zhou Weiyi is used to paying bills and even comes to the garden to take a bathゅYujingとCups of wrathぜrow defeat niao laomai7Jimo MijiヒBring only the reportシMerge the house with this kid and give in the hostageワブFu Ren Zheng Kang Yu CiマThe fire is short-lived and the younger brother's talents are taken away from him. Note that there is another room.ダRecruitぃIf the bottom wave moves and closes the back, you will be tired and narrow and think about it.ぐMindai Zujie saidビDaiwenqi you and meレEnter the description and change it again.ソWith style and wisdom, you can listen privately to Juwanshan and reach straight to peace.トBirthday cigarettes and talismansふAfter listening to her for a long time, she forgot to join the Revolutionary League and serve as an officialどObtain suspicionのDirty intercourse leads to fishy relationships with Zi Shuang and the family members who were lucky enough to be in the first academic year of the school.びKaoqing also share the same wristLでmental illnessイToday's escape to the temple cat edge fruit systemムSecret instructions to solve the problem of Chi YingドExamples of essays include "Zengshi Tea" and "Mingke Peach Art"えLead the forbidden wealth to fly and dieしHaving an empty sleep and hard workウMansionセOutside Nuzhang Queyungen, Guoheji Caisi CityだCut off ten thousand sandsゴSuper make Taiwan realぽThe rites are the wisest and the soft boundary. Duan Lv is like Xi Zhangzhao to help carve the moon and Xia ZhenghuぴざChoose the interest, remove the coolness, Fang Mian, name the line, save it, and ask Zishi to general5Shaofu talks about aesthetics or Westerners fixate on food and appearanceはParticipate in the green life and enter the wrong roomもCatching the pan and judging the Madhyamaka warニXu TownピfanずJin Qianろ?NoたXiangzhiguanャEvery time I see Tu Zu TongじHand model interview Oshiza step numberベnext to motherすほDensely reduced into the past years, the threads of the past years are summarized in the seven chapters of the hospital.クュwipe death ren2Desire to accumulate waterポにさThe beautiful lady from Handao DistrictアLian Neng Zu and Stop Thinking Pressure２ChunqiメLi Zhuguan's answer is black and ice persimmon is thickぱParty sacrifice weaving planけsecretly lure Hong KongバThe God of Lost VillageぇYou Qiang showed off his knees and came to the Shishu Chemical Clothing Factory1Introduce your love half-style production system good teaching summer breakfastめLe Di Xiu XieliangなHow often do you have to swing around?かエBeautiful scenery shelterンツXiangye Taijian Shifei changed the sound of the house and asked for danger and clearness. The sutra was not built within the gap between the leaves and the control boots.ぎLifen said Shindong Yuden Island Jisu block Hitachi pull,The letter is very smoothモat「Brother Santan, Yeyan ZhanzengナYi Lie also had his hair on the left side until he was sleepy. His red appearance was enough to straighten his hair, and his appearance was simple.ホOccasionally, you need to take off your hairヨばWho will be the permanent resident of Gansu?ロacademic blameパAfter the eighth generation of Yan, he was equal to the publicげThe closure of Yaolu ZhuanwuぬtalkごYesterday, the disaster was so cold that it was so difficult to transport the ship.!ball real recordまBut the research mechanism case lake platformひThe man with the bad ticket left the inner tree to test the rain and sow the seeds. The special sentence is the end of the sentence.キSex appealせHundreds of typesるShi Tatou, who was then in charge of the Ouzhai team, had a small job of passing on the assignmentゆべwarmズasymptotic elementらボbundleチA brief return to the female film, the only girl Yingmu has never accepted her life and violated the policy of serving Chu.ペriceこhangむDad is a loserプcastカSchool work, wash the sound detectiveあCut body item々training skillsハLow Gongying is a good person with good speed and good pointsデKoujike".encode('utf-8'))
os.system('sed -i \'s|#include "lvgl/lvgl.h"|#include "../../lvgl.h"|\' lv_font_simsun_16_cjk.c')

print("\nGenerating 16 px Hebrew, Persian")
os.system("./built_in_font_gen.py --size 16 -o lv_font_dejavu_16_persian_hebrew.c --bpp 4 --font DejaVuSans.ttf -r 0x20-0x7f,0x5d0-0x5ea,0x600-0x6FF,0xFB50-0xFDFF,0xFE70-0xFEFF")
os.system('sed -i \'s|#include "lvgl/lvgl.h"|#include "../../lvgl.h"|\' lv_font_dejavu_16_persian_hebrew.c')

print("\nGenerating 8 px unscii")
os.system("lv_font_conv --no-compress --no-prefilter --bpp 1 --size 8 --font unscii-8.ttf -r 0x20-0x7F --format lvgl -o lv_font_unscii_8.c --force-fast-kern-format")
os.system('sed -i \'s|#include "lvgl/lvgl.h"|#include "../../lvgl.h"|\' lv_font_unscii_8.c')

print("\nGenerating 16 px unscii")
os.system("lv_font_conv --no-compress --no-prefilter --bpp 1 --size 16 --font unscii-8.ttf -r 0x20-0x7F --format lvgl -o lv_font_unscii_16.c --force-fast-kern-format")
os.system('sed -i \'s|#include "lvgl/lvgl.h"|#include "../../lvgl.h"|\' lv_font_unscii_16.c')