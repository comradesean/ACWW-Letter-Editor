#include "itemdatabase.h"

namespace ItemDatabase {

namespace Detail {

// Build the complete item database
// This function is called once and the result is cached
const QMap<uint16_t, QString>& getDatabase() {
    static QMap<uint16_t, QString> database;
    static bool initialized = false;

    if (!initialized) {
        // ========================================
        // STATIONERY (0x1000 - 0x10FC)
        // Note: Stationery uses every 4th hex value
        // ========================================
        database[0x1000] = QStringLiteral("Butterfly paper");
        database[0x1004] = QStringLiteral("Airmail paper");
        database[0x1008] = QStringLiteral("New year's cards");
        database[0x100C] = QStringLiteral("Lacy paper");
        database[0x1010] = QStringLiteral("Cloudy paper");
        database[0x1014] = QStringLiteral("Petal paper");
        database[0x1018] = QStringLiteral("Snowy paper");
        database[0x101C] = QStringLiteral("Maple-leaf paper");
        database[0x1020] = QStringLiteral("Lined paper");
        database[0x1024] = QStringLiteral("Notebook paper");
        database[0x1028] = QStringLiteral("Flowery paper");
        database[0x102C] = QStringLiteral("Polka-dot paper");
        database[0x1030] = QStringLiteral("Bottle paper");
        database[0x1034] = QStringLiteral("Ribbon paper");
        database[0x1038] = QStringLiteral("Sparkly paper");
        database[0x103C] = QStringLiteral("Vine paper");
        database[0x1040] = QStringLiteral("Formal paper");
        database[0x1044] = QStringLiteral("Snowman paper");
        database[0x1048] = QStringLiteral("Card paper");
        database[0x104C] = QStringLiteral("Leopard paper");
        database[0x1050] = QStringLiteral("Cow paper");
        database[0x1054] = QStringLiteral("Camouflage paper");
        database[0x1058] = QStringLiteral("Hamburger paper");
        database[0x105C] = QStringLiteral("Piano paper");
        database[0x1060] = QStringLiteral("Nook paper");
        database[0x1064] = QStringLiteral("Fox paper");
        database[0x1068] = QStringLiteral("Birthday cards");
        database[0x106C] = QStringLiteral("Four-leaf paper");
        database[0x1070] = QStringLiteral("Town-hall paper");
        database[0x1074] = QStringLiteral("Tortimer paper");
        database[0x1078] = QStringLiteral("Insurance paper");
        database[0x107C] = QStringLiteral("Academy paper");
        database[0x1080] = QStringLiteral("Lovely paper");
        database[0x1084] = QStringLiteral("Rainbow paper");
        database[0x1088] = QStringLiteral("Egyptian paper");
        database[0x108C] = QStringLiteral("Lotus paper");
        database[0x1090] = QStringLiteral("Tile paper");
        database[0x1094] = QStringLiteral("Mosaic paper");
        database[0x1098] = QStringLiteral("Elegant paper");
        database[0x109C] = QStringLiteral("Town view paper");
        database[0x10A0] = QStringLiteral("Chinese paper");
        database[0x10A4] = QStringLiteral("Ocean paper");
        database[0x10A8] = QStringLiteral("Industrial paper");
        database[0x10AC] = QStringLiteral("Fireworks paper");
        database[0x10B0] = QStringLiteral("Floral paper");
        database[0x10B4] = QStringLiteral("Mushroom paper");
        database[0x10B8] = QStringLiteral("Star paper");
        database[0x10BC] = QStringLiteral("Composer paper");
        database[0x10C0] = QStringLiteral("Bathtub paper");
        database[0x10C4] = QStringLiteral("SMB3 paper");
        database[0x10C8] = QStringLiteral("Cool paper");
        database[0x10CC] = QStringLiteral("Forest paper");
        database[0x10D0] = QStringLiteral("Bubble paper");
        database[0x10D4] = QStringLiteral("Buttercup paper");
        database[0x10D8] = QStringLiteral("Tartan paper");
        database[0x10DC] = QStringLiteral("Plaid paper");
        database[0x10E0] = QStringLiteral("Lemon-lime paper");
        database[0x10E4] = QStringLiteral("Crater paper");
        database[0x10E8] = QStringLiteral("Bejeweled paper");
        database[0x10EC] = QStringLiteral("Geometric paper");
        database[0x10F0] = QStringLiteral("Southwest paper");
        database[0x10F4] = QStringLiteral("Night sky paper");
        database[0x10F8] = QStringLiteral("Chic paper");
        database[0x10FC] = QStringLiteral("Goldfish paper");

        // ========================================
        // WALLPAPER (0x1100 - 0x113F)
        // ========================================
        database[0x1100] = QStringLiteral("Exotic wall");
        database[0x1101] = QStringLiteral("Lovely wall");
        database[0x1102] = QStringLiteral("Classic wall");
        database[0x1103] = QStringLiteral("Ranch wall");
        database[0x1104] = QStringLiteral("Cabana wall");
        database[0x1105] = QStringLiteral("Blue wall");
        database[0x1106] = QStringLiteral("Modern wall");
        database[0x1107] = QStringLiteral("Regal wall");
        database[0x1108] = QStringLiteral("Green wall");
        database[0x1109] = QStringLiteral("Cabin wall");
        database[0x110A] = QStringLiteral("Kiddie wall");
        database[0x110B] = QStringLiteral("Robo-wall");
        database[0x110C] = QStringLiteral("Snowman wall");
        database[0x110D] = QStringLiteral("Forest wall");
        database[0x110E] = QStringLiteral("Western vista");
        database[0x110F] = QStringLiteral("Lunar horizon");
        database[0x1110] = QStringLiteral("Blue tarp");
        database[0x1111] = QStringLiteral("Ringside seating");
        database[0x1112] = QStringLiteral("Backyard fence");
        database[0x1113] = QStringLiteral("Playroom wall");
        database[0x1114] = QStringLiteral("Sea view");
        database[0x1115] = QStringLiteral("Lab wall");
        database[0x1116] = QStringLiteral("Pastel dot wall");
        database[0x1117] = QStringLiteral("Parlor wall");
        database[0x1118] = QStringLiteral("Mod wall");
        database[0x1119] = QStringLiteral("Groovy wall");
        database[0x111A] = QStringLiteral("Mosaic wall");
        database[0x111B] = QStringLiteral("Blue-trim wall");
        database[0x111C] = QStringLiteral("Plaster wall");
        database[0x111D] = QStringLiteral("Chic wall");
        database[0x111E] = QStringLiteral("Ornate wall");
        database[0x111F] = QStringLiteral("Citrus wall");
        database[0x1120] = QStringLiteral("Rose wall");
        database[0x1121] = QStringLiteral("Old brick wall");
        database[0x1122] = QStringLiteral("Shoji screen");
        database[0x1123] = QStringLiteral("Stone wall");
        database[0x1124] = QStringLiteral("Concrete wall");
        database[0x1125] = QStringLiteral("Shanty wall");
        database[0x1126] = QStringLiteral("Wood paneling");
        database[0x1127] = QStringLiteral("Floral wall");
        database[0x1128] = QStringLiteral("Music room wall");
        database[0x1129] = QStringLiteral("Manor wall");
        database[0x112A] = QStringLiteral("Plaza wall");
        database[0x112B] = QStringLiteral("Lattice wall");
        database[0x112C] = QStringLiteral("Stately wall");
        database[0x112D] = QStringLiteral("Imperial wall");
        database[0x112E] = QStringLiteral("Arched window");
        database[0x112F] = QStringLiteral("Bamboo wall");
        database[0x1130] = QStringLiteral("Ivy wall");
        database[0x1131] = QStringLiteral("Ancient wall");
        database[0x1132] = QStringLiteral("Exquisite wall");
        database[0x1133] = QStringLiteral("Persian wall");
        database[0x1134] = QStringLiteral("Industrial wall");
        database[0x1135] = QStringLiteral("Library wall");
        database[0x1136] = QStringLiteral("Fairy tale wall");
        database[0x1137] = QStringLiteral("Illusion wall");
        database[0x1138] = QStringLiteral("Meadow vista");
        database[0x1139] = QStringLiteral("Tropical vista");
        database[0x113A] = QStringLiteral("Desert vista");
        database[0x113B] = QStringLiteral("Chainlink fence");
        database[0x113C] = QStringLiteral("Sky wall");
        database[0x113D] = QStringLiteral("Cityscape wall");
        database[0x113E] = QStringLiteral("Common wall");
        database[0x113F] = QStringLiteral("Mushroom mural");

        // ========================================
        // CARPET/FLOORING (0x1144 - 0x1183)
        // ========================================
        database[0x1144] = QStringLiteral("Exotic rug");
        database[0x1145] = QStringLiteral("Lovely carpet");
        database[0x1146] = QStringLiteral("Classic carpet");
        database[0x1147] = QStringLiteral("Ranch flooring");
        database[0x1148] = QStringLiteral("Cabana flooring");
        database[0x1149] = QStringLiteral("Blue flooring");
        database[0x114A] = QStringLiteral("Modern tile");
        database[0x114B] = QStringLiteral("Regal carpet");
        database[0x114C] = QStringLiteral("Green rug");
        database[0x114D] = QStringLiteral("Cabin rug");
        database[0x114E] = QStringLiteral("Kiddie carpet");
        database[0x114F] = QStringLiteral("Robo-floor");
        database[0x1150] = QStringLiteral("Snowman carpet");
        database[0x1151] = QStringLiteral("Forest floor");
        database[0x1152] = QStringLiteral("Western desert");
        database[0x1153] = QStringLiteral("Lunar surface");
        database[0x1154] = QStringLiteral("Closed road");
        database[0x1155] = QStringLiteral("Boxing ring mat");
        database[0x1156] = QStringLiteral("Backyard lawn");
        database[0x1157] = QStringLiteral("Playroom rug");
        database[0x1158] = QStringLiteral("Ship deck");
        database[0x1159] = QStringLiteral("Lab floor");
        database[0x115A] = QStringLiteral("Pastel dot rug");
        database[0x115B] = QStringLiteral("Checkered tile");
        database[0x115C] = QStringLiteral("Ceramic tile");
        database[0x115D] = QStringLiteral("Diner tile");
        database[0x115E] = QStringLiteral("Red tile");
        database[0x115F] = QStringLiteral("Mosaic tile");
        database[0x1160] = QStringLiteral("Plaza tile");
        database[0x1161] = QStringLiteral("Palace tile");
        database[0x1162] = QStringLiteral("Imperial tile");
        database[0x1163] = QStringLiteral("Kitchen tile");
        database[0x1164] = QStringLiteral("Stone tile");
        database[0x1165] = QStringLiteral("Charcoal tile");
        database[0x1166] = QStringLiteral("Tatami floor");
        database[0x1167] = QStringLiteral("Slate flooring");
        database[0x1168] = QStringLiteral("Concrete floor");
        database[0x1169] = QStringLiteral("Parquet floor");
        database[0x116A] = QStringLiteral("Music room floor");
        database[0x116B] = QStringLiteral("Opulent rug");
        database[0x116C] = QStringLiteral("Plush carpet");
        database[0x116D] = QStringLiteral("Ornate rug");
        database[0x116E] = QStringLiteral("Fancy carpet");
        database[0x116F] = QStringLiteral("Exquisite rug");
        database[0x1170] = QStringLiteral("Persian rug");
        database[0x1171] = QStringLiteral("Tartan rug");
        database[0x1172] = QStringLiteral("Round carpet");
        database[0x1173] = QStringLiteral("Citrus carpet");
        database[0x1174] = QStringLiteral("Cowhide rug");
        database[0x1175] = QStringLiteral("Birch flooring");
        database[0x1176] = QStringLiteral("Old flooring");
        database[0x1177] = QStringLiteral("Bamboo flooring");
        database[0x1178] = QStringLiteral("Steel flooring");
        database[0x1179] = QStringLiteral("Shanty mat");
        database[0x117A] = QStringLiteral("Ancient tile");
        database[0x117B] = QStringLiteral("Fairy tale floor");
        database[0x117C] = QStringLiteral("Illusion floor");
        database[0x117D] = QStringLiteral("Daisy meadow");
        database[0x117E] = QStringLiteral("Tropical floor");
        database[0x117F] = QStringLiteral("Saharah's desert");
        database[0x1180] = QStringLiteral("Sandlot");
        database[0x1181] = QStringLiteral("Cloud flooring");
        database[0x1182] = QStringLiteral("Common floor");
        database[0x1183] = QStringLiteral("Block floor");

        // ========================================
        // CLOTHING (0x11A8 - 0x12A7)
        // ========================================
        database[0x11A8] = QStringLiteral("Work uniform");
        database[0x11A9] = QStringLiteral("One-ball shirt");
        database[0x11AA] = QStringLiteral("Two-ball shirt");
        database[0x11AB] = QStringLiteral("Three-ball shirt");
        database[0x11AC] = QStringLiteral("Four-ball shirt");
        database[0x11AD] = QStringLiteral("Five-ball shirt");
        database[0x11AE] = QStringLiteral("Six-ball shirt");
        database[0x11AF] = QStringLiteral("Seven-ball shirt");
        database[0x11B0] = QStringLiteral("Eight-ball shirt");
        database[0x11B1] = QStringLiteral("Nine-ball shirt");
        database[0x11B2] = QStringLiteral("Paw shirt");
        database[0x11B3] = QStringLiteral("Daisy shirt");
        database[0x11B4] = QStringLiteral("Tulip shirt");
        database[0x11B5] = QStringLiteral("Cherry shirt");
        database[0x11B6] = QStringLiteral("Skull shirt");
        database[0x11B7] = QStringLiteral("U R here shirt");
        database[0x11B8] = QStringLiteral("Lightning shirt");
        database[0x11B9] = QStringLiteral("MVP shirt");
        database[0x11BA] = QStringLiteral("BB shirt");
        database[0x11BB] = QStringLiteral("Frog shirt");
        database[0x11BC] = QStringLiteral("Bear shirt");
        database[0x11BD] = QStringLiteral("Bunny shirt");
        database[0x11BE] = QStringLiteral("Elephant shirt");
        database[0x11BF] = QStringLiteral("Spade shirt");
        database[0x11C0] = QStringLiteral("Diamond shirt");
        database[0x11C1] = QStringLiteral("Club shirt");
        database[0x11C2] = QStringLiteral("Heart shirt");
        database[0x11C3] = QStringLiteral("Big star shirt");
        database[0x11C4] = QStringLiteral("Bright shirt");
        database[0x11C5] = QStringLiteral("A shirt");
        database[0x11C6] = QStringLiteral("No. 1 shirt");
        database[0x11C7] = QStringLiteral("No. 2 shirt");
        database[0x11C8] = QStringLiteral("No. 3 shirt");
        database[0x11C9] = QStringLiteral("No. 4 shirt");
        database[0x11CA] = QStringLiteral("No. 5 shirt");
        database[0x11CB] = QStringLiteral("No. 23 shirt");
        database[0x11CC] = QStringLiteral("No. 67 shirt");
        database[0x11CD] = QStringLiteral("Big bro's shirt");
        database[0x11CE] = QStringLiteral("Li'l bro's shirt");
        database[0x11CF] = QStringLiteral("Cloudy shirt");
        database[0x11D0] = QStringLiteral("Fresh shirt");
        database[0x11D1] = QStringLiteral("Dawn shirt");
        database[0x11D2] = QStringLiteral("Misty shirt");
        database[0x11D3] = QStringLiteral("Sunset top");
        database[0x11D4] = QStringLiteral("Deep blue tee");
        database[0x11D5] = QStringLiteral("Peachy shirt");
        database[0x11D6] = QStringLiteral("Rainbow shirt");
        database[0x11D7] = QStringLiteral("Snowcone shirt");
        database[0x11D8] = QStringLiteral("Orange tie-dye");
        database[0x11D9] = QStringLiteral("Purple tie-dye");
        database[0x11DA] = QStringLiteral("Green tie-dye");
        database[0x11DB] = QStringLiteral("Blue tie-dye");
        database[0x11DC] = QStringLiteral("Red tie-dye");
        database[0x11DD] = QStringLiteral("Bold check shirt");
        database[0x11DE] = QStringLiteral("Cafe shirt");
        database[0x11DF] = QStringLiteral("Checkered shirt");
        database[0x11E0] = QStringLiteral("Blue check shirt");
        database[0x11E1] = QStringLiteral("Red check shirt");
        database[0x11E2] = QStringLiteral("Rugby shirt");
        database[0x11E3] = QStringLiteral("Green bar shirt");
        database[0x11E4] = QStringLiteral("Yellow bar shirt");
        database[0x11E5] = QStringLiteral("Grape stripe tee");
        database[0x11E6] = QStringLiteral("Beatnik shirt");
        database[0x11E7] = QStringLiteral("Red bar shirt");
        database[0x11E8] = QStringLiteral("Blue stripe knit");
        database[0x11E9] = QStringLiteral("Gelato shirt");
        database[0x11EA] = QStringLiteral("Chain-gang shirt");
        database[0x11EB] = QStringLiteral("Yellow tartan");
        database[0x11EC] = QStringLiteral("Fall plaid shirt");
        database[0x11ED] = QStringLiteral("Blue tartan");
        database[0x11EE] = QStringLiteral("Dapper shirt");
        database[0x11EF] = QStringLiteral("Natty shirt");
        database[0x11F0] = QStringLiteral("Blue grid shirt");
        database[0x11F1] = QStringLiteral("Red grid shirt");
        database[0x11F2] = QStringLiteral("Dazed shirt");
        database[0x11F3] = QStringLiteral("Checkerboard tee");
        database[0x11F4] = QStringLiteral("Toad shirt");
        database[0x11F5] = QStringLiteral("Dark polka shirt");
        database[0x11F6] = QStringLiteral("Lite polka shirt");
        database[0x11F7] = QStringLiteral("Bubble gum shirt");
        database[0x11F8] = QStringLiteral("Funky dot shirt");
        database[0x11F9] = QStringLiteral("Gumdrop shirt");
        database[0x11FA] = QStringLiteral("Big dot shirt");
        database[0x11FB] = QStringLiteral("Aqua polka shirt");
        database[0x11FC] = QStringLiteral("Blue pinstripe");
        database[0x11FD] = QStringLiteral("Yellow pinstripe");
        database[0x11FE] = QStringLiteral("Orange pinstripe");
        database[0x11FF] = QStringLiteral("Vegetarian shirt");
        database[0x1200] = QStringLiteral("Racer shirt");
        database[0x1201] = QStringLiteral("Vertigo shirt");
        database[0x1202] = QStringLiteral("Barber shirt");
        database[0x1203] = QStringLiteral("Jade check print");
        database[0x1204] = QStringLiteral("Blue check print");
        database[0x1205] = QStringLiteral("Mint gingham top");
        database[0x1206] = QStringLiteral("Picnic shirt");
        database[0x1207] = QStringLiteral("Candy gingham");
        database[0x1208] = QStringLiteral("Lemon gingham");
        database[0x1209] = QStringLiteral("Melon gingham");
        database[0x120A] = QStringLiteral("Bad plaid shirt");
        database[0x120B] = QStringLiteral("Pink tartan");
        database[0x120C] = QStringLiteral("Waffle shirt");
        database[0x120D] = QStringLiteral("Gray tartan");
        database[0x120E] = QStringLiteral("Chevron shirt");
        database[0x120F] = QStringLiteral("Icy shirt");
        database[0x1210] = QStringLiteral("Aurora knit");
        database[0x1211] = QStringLiteral("Winter sweater");
        database[0x1212] = QStringLiteral("Folk shirt");
        database[0x1213] = QStringLiteral("Argyle knit");
        database[0x1214] = QStringLiteral("Uncommon shirt");
        database[0x1215] = QStringLiteral("Comfy sweater");
        database[0x1216] = QStringLiteral("Beige knit");
        database[0x1217] = QStringLiteral("Earthy knit");
        database[0x1218] = QStringLiteral("Spring shirt");
        database[0x1219] = QStringLiteral("Vogue top");
        database[0x121A] = QStringLiteral("Citrus gingham");
        database[0x121B] = QStringLiteral("Floral knit");
        database[0x121C] = QStringLiteral("Dreamy shirt");
        database[0x121D] = QStringLiteral("Flowery shirt");
        database[0x121E] = QStringLiteral("Silk bloom shirt");
        database[0x121F] = QStringLiteral("Pop bloom shirt");
        database[0x1220] = QStringLiteral("Blossom shirt");
        database[0x1221] = QStringLiteral("Loud bloom shirt");
        database[0x1222] = QStringLiteral("Rose shirt");
        database[0x1223] = QStringLiteral("Rose sky shirt");
        database[0x1224] = QStringLiteral("Lotus shirt");
        database[0x1225] = QStringLiteral("Chocomint shirt");
        database[0x1226] = QStringLiteral("Fern shirt");
        database[0x1227] = QStringLiteral("Blue retro shirt");
        database[0x1228] = QStringLiteral("Orange check tee");
        database[0x1229] = QStringLiteral("Leaf shirt");
        database[0x122A] = QStringLiteral("Fall leaf shirt");
        database[0x122B] = QStringLiteral("Grass shirt");
        database[0x122C] = QStringLiteral("Snow shirt");
        database[0x122D] = QStringLiteral("Lovely shirt");
        database[0x122E] = QStringLiteral("Bubble shirt");
        database[0x122F] = QStringLiteral("Chichi print");
        database[0x1230] = QStringLiteral("Coral shirt");
        database[0x1231] = QStringLiteral("Groovy shirt");
        database[0x1232] = QStringLiteral("Cool shirt");
        database[0x1233] = QStringLiteral("Swell shirt");
        database[0x1234] = QStringLiteral("Blue diamond top");
        database[0x1235] = QStringLiteral("Prism shirt");
        database[0x1236] = QStringLiteral("Go-go shirt");
        database[0x1237] = QStringLiteral("Flame shirt");
        database[0x1238] = QStringLiteral("Danger shirt");
        database[0x1239] = QStringLiteral("Gracie's top");
        database[0x123A] = QStringLiteral("Future shirt");
        database[0x123B] = QStringLiteral("Optical shirt");
        database[0x123C] = QStringLiteral("Twinkle shirt");
        database[0x123D] = QStringLiteral("Star shirt");
        database[0x123E] = QStringLiteral("Night sky tee");
        database[0x123F] = QStringLiteral("Amethyst shirt");
        database[0x1240] = QStringLiteral("Nebula shirt");
        database[0x1241] = QStringLiteral("Dice shirt");
        database[0x1242] = QStringLiteral("Kiddie shirt");
        database[0x1243] = QStringLiteral("Airy shirt");
        database[0x1244] = QStringLiteral("Polar fleece");
        database[0x1245] = QStringLiteral("Crossing shirt");
        database[0x1246] = QStringLiteral("Splendid shirt");
        database[0x1247] = QStringLiteral("Jagged shirt");
        database[0x1248] = QStringLiteral("Subdued print");
        database[0x1249] = QStringLiteral("Sharp outfit");
        database[0x124A] = QStringLiteral("Jungle camo");
        database[0x124B] = QStringLiteral("Arctic camo");
        database[0x124C] = QStringLiteral("Desert camo");
        database[0x124D] = QStringLiteral("Zebra shirt");
        database[0x124E] = QStringLiteral("Tiger shirt");
        database[0x124F] = QStringLiteral("Cow shirt");
        database[0x1250] = QStringLiteral("Leopard shirt");
        database[0x1251] = QStringLiteral("Giraffe shirt");
        database[0x1252] = QStringLiteral("Ladybug shirt");
        database[0x1253] = QStringLiteral("Butterfly shirt");
        database[0x1254] = QStringLiteral("Spiderweb shirt");
        database[0x1255] = QStringLiteral("Caterpillar tee");
        database[0x1256] = QStringLiteral("Fiendish shirt");
        database[0x1257] = QStringLiteral("Citrus shirt");
        database[0x1258] = QStringLiteral("Kiwi shirt");
        database[0x1259] = QStringLiteral("Watermelon shirt");
        database[0x125A] = QStringLiteral("Strawberry shirt");
        database[0x125B] = QStringLiteral("Grape shirt");
        database[0x125C] = QStringLiteral("Melon shirt");
        database[0x125D] = QStringLiteral("Pink wave shirt");
        database[0x125E] = QStringLiteral("Flan shirt");
        database[0x125F] = QStringLiteral("Hot dog shirt");
        database[0x1260] = QStringLiteral("Sandwich shirt");
        database[0x1261] = QStringLiteral("Dragon suit");
        database[0x1262] = QStringLiteral("Asian shirt");
        database[0x1263] = QStringLiteral("Hot spring shirt");
        database[0x1264] = QStringLiteral("New spring shirt");
        database[0x1265] = QStringLiteral("Crewel shirt");
        database[0x1266] = QStringLiteral("Tropical shirt");
        database[0x1267] = QStringLiteral("Ribbon shirt");
        database[0x1268] = QStringLiteral("Bodice");
        database[0x1269] = QStringLiteral("Laced shirt");
        database[0x126A] = QStringLiteral("Circus shirt");
        database[0x126B] = QStringLiteral("Green vest");
        database[0x126C] = QStringLiteral("Yellow bolero");
        database[0x126D] = QStringLiteral("Noble shirt");
        database[0x126E] = QStringLiteral("Turnip top");
        database[0x126F] = QStringLiteral("Yodel shirt");
        database[0x1270] = QStringLiteral("Gaudy sweater");
        database[0x1271] = QStringLiteral("Western shirt");
        database[0x1272] = QStringLiteral("Red riding hoody");
        database[0x1273] = QStringLiteral("Royal shirt");
        database[0x1274] = QStringLiteral("Witch shirt");
        database[0x1275] = QStringLiteral("Firefighter tee");
        database[0x1276] = QStringLiteral("Graduation gown");
        database[0x1277] = QStringLiteral("Sky shirt");
        database[0x1278] = QStringLiteral("Captain's shirt");
        database[0x1279] = QStringLiteral("Burglar's shirt");
        database[0x127A] = QStringLiteral("Jester shirt");
        database[0x127B] = QStringLiteral("Nurse's uniform");
        database[0x127C] = QStringLiteral("Bone shirt");
        database[0x127D] = QStringLiteral("Zipper shirt");
        database[0x127E] = QStringLiteral("Mummy shirt");
        database[0x127F] = QStringLiteral("Military uniform");
        database[0x1280] = QStringLiteral("Sailor's uniform");
        database[0x1281] = QStringLiteral("Reggae shirt");
        database[0x1282] = QStringLiteral("Camel shirt");
        database[0x1283] = QStringLiteral("Molecule tee");
        database[0x1284] = QStringLiteral("Concierge shirt");
        database[0x1285] = QStringLiteral("Kimono");
        database[0x1286] = QStringLiteral("Tuxedo");
        database[0x1287] = QStringLiteral("Explorer shirt");
        database[0x1288] = QStringLiteral("Dutch shirt");
        database[0x1289] = QStringLiteral("Toga");
        database[0x128A] = QStringLiteral("Cake shirt");
        database[0x128B] = QStringLiteral("Waitress shirt");
        database[0x128C] = QStringLiteral("Princess shirt");
        database[0x128D] = QStringLiteral("Fairy tale shirt");
        database[0x128E] = QStringLiteral("Spanish shirt");
        database[0x128F] = QStringLiteral("Latin uniform");
        database[0x1290] = QStringLiteral("Red down vest");
        database[0x1291] = QStringLiteral("One-way tee");
        database[0x1292] = QStringLiteral("Beaded shirt");
        database[0x1293] = QStringLiteral("Nile shirt");
        database[0x1294] = QStringLiteral("Wrap shirt");
        database[0x1295] = QStringLiteral("Wrestler shirt");
        database[0x1296] = QStringLiteral("Security shirt");
        database[0x1297] = QStringLiteral("Poncho");
        database[0x1298] = QStringLiteral("Fluffy shirt");
        database[0x1299] = QStringLiteral("Chinese shirt");
        database[0x129A] = QStringLiteral("Pep squad shirt");
        database[0x129B] = QStringLiteral("Racing shirt");
        database[0x129C] = QStringLiteral("Orange jumpsuit");
        database[0x129D] = QStringLiteral("Tin shirt");
        database[0x129E] = QStringLiteral("Scale armor suit");
        database[0x129F] = QStringLiteral("Armor suit");
        database[0x12A0] = QStringLiteral("Gold armor suit");
        database[0x12A1] = QStringLiteral("Red warmup suit");
        database[0x12A2] = QStringLiteral("Baseball shirt");
        database[0x12A3] = QStringLiteral("Leather jerkin");
        database[0x12A4] = QStringLiteral("Frock coat");
        database[0x12A5] = QStringLiteral("Space suit");
        database[0x12A6] = QStringLiteral("Caveman tunic");
        database[0x12A7] = QStringLiteral("Moldy shirt");

        // ========================================
        // INSECTS (0x12B0 - 0x12E7)
        // ========================================
        database[0x12B0] = QStringLiteral("Common butterfly");
        database[0x12B1] = QStringLiteral("Yellow butterfly");
        database[0x12B2] = QStringLiteral("Tiger butterfly");
        database[0x12B3] = QStringLiteral("Peacock");
        database[0x12B4] = QStringLiteral("Monarch");
        database[0x12B5] = QStringLiteral("Emperor");
        database[0x12B6] = QStringLiteral("Agrias butterfly");
        database[0x12B7] = QStringLiteral("Birdwing");
        database[0x12B8] = QStringLiteral("Moth");
        database[0x12B9] = QStringLiteral("Oak silk moth");
        database[0x12BA] = QStringLiteral("Honeybee");
        database[0x12BB] = QStringLiteral("Bee");
        database[0x12BC] = QStringLiteral("Long locust");
        database[0x12BD] = QStringLiteral("Migratory locust");
        database[0x12BE] = QStringLiteral("Mantis");
        database[0x12BF] = QStringLiteral("Orchid mantis");
        database[0x12C0] = QStringLiteral("Brown cicada");
        database[0x12C1] = QStringLiteral("Robust cicada");
        database[0x12C2] = QStringLiteral("Walker cicada");
        database[0x12C3] = QStringLiteral("Evening cicada");
        database[0x12C4] = QStringLiteral("Lantern fly");
        database[0x12C5] = QStringLiteral("Red dragonfly");
        database[0x12C6] = QStringLiteral("Darner dragonfly");
        database[0x12C7] = QStringLiteral("Banded dragonfly");
        database[0x12C8] = QStringLiteral("Ant");
        database[0x12C9] = QStringLiteral("Pondskater");
        database[0x12CA] = QStringLiteral("Snail");
        database[0x12CB] = QStringLiteral("Cricket");
        database[0x12CC] = QStringLiteral("Bell cricket");
        database[0x12CD] = QStringLiteral("Grasshopper");
        database[0x12CE] = QStringLiteral("Mole cricket");
        database[0x12CF] = QStringLiteral("Walkingstick");
        database[0x12D0] = QStringLiteral("Ladybug");
        database[0x12D1] = QStringLiteral("Fruit beetle");
        database[0x12D2] = QStringLiteral("Scarab beetle");
        database[0x12D3] = QStringLiteral("Dung beetle");
        database[0x12D4] = QStringLiteral("Goliath beetle");
        database[0x12D5] = QStringLiteral("Firefly");
        database[0x12D6] = QStringLiteral("Jewel beetle");
        database[0x12D7] = QStringLiteral("Longhorn beetle");
        database[0x12D8] = QStringLiteral("Saw stag beetle");
        database[0x12D9] = QStringLiteral("Stag beetle");
        database[0x12DA] = QStringLiteral("Giant beetle");
        database[0x12DB] = QStringLiteral("Rainbow stag");
        database[0x12DC] = QStringLiteral("Dynastid beetle");
        database[0x12DD] = QStringLiteral("Atlas beetle");
        database[0x12DE] = QStringLiteral("Elephant beetle");
        database[0x12DF] = QStringLiteral("Hercules beetle");
        database[0x12E0] = QStringLiteral("Flea");
        database[0x12E1] = QStringLiteral("Pill bug");
        database[0x12E2] = QStringLiteral("Mosquito");
        database[0x12E3] = QStringLiteral("Fly");
        database[0x12E4] = QStringLiteral("Cockroach");
        database[0x12E5] = QStringLiteral("Spider");
        database[0x12E6] = QStringLiteral("Tarantula");
        database[0x12E7] = QStringLiteral("Scorpion");

        // ========================================
        // FISH (0x12E8 - 0x131F)
        // ========================================
        database[0x12E8] = QStringLiteral("Bitterling");
        database[0x12E9] = QStringLiteral("Pale chub");
        database[0x12EA] = QStringLiteral("Crucian carp");
        database[0x12EB] = QStringLiteral("Dace");
        database[0x12EC] = QStringLiteral("Barbel steed");
        database[0x12ED] = QStringLiteral("Carp");
        database[0x12EE] = QStringLiteral("Koi");
        database[0x12EF] = QStringLiteral("Goldfish");
        database[0x12F0] = QStringLiteral("Popeyed goldfish");
        database[0x12F1] = QStringLiteral("Killifish");
        database[0x12F2] = QStringLiteral("Crawfish");
        database[0x12F3] = QStringLiteral("Frog");
        database[0x12F4] = QStringLiteral("Freshwater goby");
        database[0x12F5] = QStringLiteral("Loach");
        database[0x12F6] = QStringLiteral("Catfish");
        database[0x12F7] = QStringLiteral("Eel");
        database[0x12F8] = QStringLiteral("Giant snakehead");
        database[0x12F9] = QStringLiteral("Bluegill");
        database[0x12FA] = QStringLiteral("Yellow perch");
        database[0x12FB] = QStringLiteral("Black bass");
        database[0x12FC] = QStringLiteral("Pond smelt");
        database[0x12FD] = QStringLiteral("Sweetfish");
        database[0x12FE] = QStringLiteral("Cherry salmon");
        database[0x12FF] = QStringLiteral("Char");
        database[0x1300] = QStringLiteral("Rainbow trout");
        database[0x1301] = QStringLiteral("Stringfish");
        database[0x1302] = QStringLiteral("Salmon");
        database[0x1303] = QStringLiteral("King salmon");
        database[0x1304] = QStringLiteral("Guppy");
        database[0x1305] = QStringLiteral("Angelfish");
        database[0x1306] = QStringLiteral("Piranha");
        database[0x1307] = QStringLiteral("Arowana");
        database[0x1308] = QStringLiteral("Dorado");
        database[0x1309] = QStringLiteral("Gar");
        database[0x130A] = QStringLiteral("Arapaima");
        database[0x130B] = QStringLiteral("Sea butterfly");
        database[0x130C] = QStringLiteral("Jellyfish");
        database[0x130D] = QStringLiteral("Seahorse");
        database[0x130E] = QStringLiteral("Clownfish");
        database[0x130F] = QStringLiteral("Zebra turkeyfish");
        database[0x1310] = QStringLiteral("Puffer fish");
        database[0x1311] = QStringLiteral("Horse mackerel");
        database[0x1312] = QStringLiteral("Barred knifejaw");
        database[0x1313] = QStringLiteral("Sea bass");
        database[0x1314] = QStringLiteral("Red snapper");
        database[0x1315] = QStringLiteral("Dab");
        database[0x1316] = QStringLiteral("Olive flounder");
        database[0x1317] = QStringLiteral("Squid");
        database[0x1318] = QStringLiteral("Octopus");
        database[0x1319] = QStringLiteral("Football fish");
        database[0x131A] = QStringLiteral("Tuna");
        database[0x131B] = QStringLiteral("Blue marlin");
        database[0x131C] = QStringLiteral("Ocean sunfish");
        database[0x131D] = QStringLiteral("Hammerhead shark");
        database[0x131E] = QStringLiteral("Shark");
        database[0x131F] = QStringLiteral("Coelacanth");

        // ========================================
        // TRASH (0x1320 - 0x1322)
        // ========================================
        database[0x1320] = QStringLiteral("Empty can");
        database[0x1321] = QStringLiteral("Boot");
        database[0x1322] = QStringLiteral("Old tire");

        // ========================================
        // K.K. SLIDER SONGS (0x1323 - 0x1368)
        // ========================================
        database[0x1323] = QStringLiteral("K.K. Chorale");
        database[0x1324] = QStringLiteral("K.K. March");
        database[0x1325] = QStringLiteral("K.K. Waltz");
        database[0x1326] = QStringLiteral("K.K. Swing");
        database[0x1327] = QStringLiteral("K.K. Jazz");
        database[0x1328] = QStringLiteral("K.K. Fusion");
        database[0x1329] = QStringLiteral("K.K. Etude");
        database[0x132A] = QStringLiteral("K.K. Lullaby");
        database[0x132B] = QStringLiteral("K.K. Aria");
        database[0x132C] = QStringLiteral("K.K. Samba");
        database[0x132D] = QStringLiteral("K.K. Bossa");
        database[0x132E] = QStringLiteral("K.K. Calypso");
        database[0x132F] = QStringLiteral("K.K. Salsa");
        database[0x1330] = QStringLiteral("K.K. Mambo");
        database[0x1331] = QStringLiteral("K.K. Reggae");
        database[0x1332] = QStringLiteral("K.K. Ska");
        database[0x1333] = QStringLiteral("K.K. Tango");
        database[0x1334] = QStringLiteral("K.K. Faire");
        database[0x1335] = QStringLiteral("Aloha K.K.");
        database[0x1336] = QStringLiteral("Lucky K.K.");
        database[0x1337] = QStringLiteral("K.K. Condor");
        database[0x1338] = QStringLiteral("K.K. Steppe");
        database[0x1339] = QStringLiteral("Imperial K.K.");
        database[0x133A] = QStringLiteral("K.K. Casbah");
        database[0x133B] = QStringLiteral("K.K. Safari");
        database[0x133C] = QStringLiteral("K.K. Folk");
        database[0x133D] = QStringLiteral("K.K. Rock");
        database[0x133E] = QStringLiteral("Rockin' K.K.");
        database[0x133F] = QStringLiteral("K.K. Ragtime");
        database[0x1340] = QStringLiteral("K.K. Gumbo");
        database[0x1341] = QStringLiteral("The K. Funk");
        database[0x1342] = QStringLiteral("K.K. Blues");
        database[0x1343] = QStringLiteral("Soulful K.K.");
        database[0x1344] = QStringLiteral("K.K. Soul");
        database[0x1345] = QStringLiteral("K.K. Cruisin'");
        database[0x1346] = QStringLiteral("K.K. Love Song");
        database[0x1347] = QStringLiteral("K.K. D & B");
        database[0x1348] = QStringLiteral("K.K. Technopop");
        database[0x1349] = QStringLiteral("DJ K.K.");
        database[0x134A] = QStringLiteral("Only Me");
        database[0x134B] = QStringLiteral("K.K. Country");
        database[0x134C] = QStringLiteral("Surfin' K.K.");
        database[0x134D] = QStringLiteral("K.K. Ballad");
        database[0x134E] = QStringLiteral("Comrade K.K.");
        database[0x134F] = QStringLiteral("K.K. Lament");
        database[0x1350] = QStringLiteral("Go K.K. Rider!");
        database[0x1351] = QStringLiteral("K.K. Dirge");
        database[0x1352] = QStringLiteral("K.K. Western");
        database[0x1353] = QStringLiteral("Mr. K.K.");
        database[0x1354] = QStringLiteral("Cafe K.K.");
        database[0x1355] = QStringLiteral("K.K. Parade");
        database[0x1356] = QStringLiteral("Senor K.K.");
        database[0x1357] = QStringLiteral("K.K. Song");
        database[0x1358] = QStringLiteral("I Love You");
        database[0x1359] = QStringLiteral("Two Days Ago");
        database[0x135A] = QStringLiteral("My Place");
        database[0x135B] = QStringLiteral("Forest Life");
        database[0x135C] = QStringLiteral("To the Edge");
        database[0x135D] = QStringLiteral("Pondering");
        database[0x135E] = QStringLiteral("K.K. Dixie");
        database[0x135F] = QStringLiteral("K.K. Marathon");
        database[0x1360] = QStringLiteral("King K.K.");
        database[0x1361] = QStringLiteral("Mountain Song");
        database[0x1362] = QStringLiteral("Marine Song 2001");
        database[0x1363] = QStringLiteral("Neapolitan");
        database[0x1364] = QStringLiteral("Steep Hill");
        database[0x1365] = QStringLiteral("K.K. Rockabilly");
        database[0x1366] = QStringLiteral("Agent K.K.");
        database[0x1367] = QStringLiteral("K.K. Rally");
        database[0x1368] = QStringLiteral("K.K. Metal");

        // ========================================
        // TOOLS (0x1369 - 0x137F)
        // ========================================
        database[0x1369] = QStringLiteral("Shovel");
        database[0x136A] = QStringLiteral("Golden shovel");
        database[0x136B] = QStringLiteral("Axe");
        database[0x136C] = QStringLiteral("Axe");
        database[0x136D] = QStringLiteral("Axe");
        database[0x136E] = QStringLiteral("Axe");
        database[0x136F] = QStringLiteral("Axe");
        database[0x1370] = QStringLiteral("Axe");
        database[0x1371] = QStringLiteral("Axe");
        database[0x1372] = QStringLiteral("Axe");
        database[0x1373] = QStringLiteral("Golden axe");
        database[0x1374] = QStringLiteral("Fishing rod");
        database[0x1375] = QStringLiteral("Golden rod");
        database[0x1376] = QStringLiteral("Net");
        database[0x1377] = QStringLiteral("Golden net");
        database[0x1378] = QStringLiteral("Watering can");
        database[0x1379] = QStringLiteral("Golden can");
        database[0x137A] = QStringLiteral("Slingshot");
        database[0x137B] = QStringLiteral("Golden slingshot");
        database[0x137C] = QStringLiteral("Dandelion puffs");
        database[0x137D] = QStringLiteral("Party popper");
        database[0x137E] = QStringLiteral("Sparkler");
        database[0x137F] = QStringLiteral("Roman candle");

        // ========================================
        // UMBRELLAS (0x1380 - 0x139F)
        // ========================================
        database[0x1380] = QStringLiteral("Gelato umbrella");
        database[0x1381] = QStringLiteral("Bat umbrella");
        database[0x1382] = QStringLiteral("Lacy parasol");
        database[0x1383] = QStringLiteral("Leaf umbrella");
        database[0x1384] = QStringLiteral("Paper parasol");
        database[0x1385] = QStringLiteral("Ribbon umbrella");
        database[0x1386] = QStringLiteral("Red umbrella");
        database[0x1387] = QStringLiteral("Blue umbrella");
        database[0x1388] = QStringLiteral("Yellow umbrella");
        database[0x1389] = QStringLiteral("Green umbrella");
        database[0x138A] = QStringLiteral("Candy umbrella");
        database[0x138B] = QStringLiteral("Melon umbrella");
        database[0x138C] = QStringLiteral("Mint umbrella");
        database[0x138D] = QStringLiteral("Picnic umbrella");
        database[0x138E] = QStringLiteral("Lemon umbrella");
        database[0x138F] = QStringLiteral("Toad parasol");
        database[0x1390] = QStringLiteral("Eggy parasol");
        database[0x1391] = QStringLiteral("Blue dot parasol");
        database[0x1392] = QStringLiteral("Daisy umbrella");
        database[0x1393] = QStringLiteral("Paw umbrella");
        database[0x1394] = QStringLiteral("Petal parasol");
        database[0x1395] = QStringLiteral("Busted umbrella");
        database[0x1396] = QStringLiteral("Sunny parasol");
        database[0x1397] = QStringLiteral("Beach umbrella");
        database[0x1398] = QStringLiteral("Elegant umbrella");
        database[0x1399] = QStringLiteral("Modern umbrella");
        database[0x139A] = QStringLiteral("Leopard umbrella");
        database[0x139B] = QStringLiteral("Zebra umbrella");
        database[0x139C] = QStringLiteral("Forest umbrella");
        database[0x139D] = QStringLiteral("Flame umbrella");
        database[0x139E] = QStringLiteral("Camo umbrella");
        database[0x139F] = QStringLiteral("Spider umbrella");

        // ========================================
        // HATS/WIGS/HELMETS (0x13A8 - 0x13F7)
        // ========================================
        database[0x13A8] = QStringLiteral("Pilot's cap");
        database[0x13A9] = QStringLiteral("Fireman's hat");
        database[0x13AA] = QStringLiteral("Baby's hat");
        database[0x13AB] = QStringLiteral("White veil");
        database[0x13AC] = QStringLiteral("Turban");
        database[0x13AD] = QStringLiteral("Warbonnet");
        database[0x13AE] = QStringLiteral("Kaffiyeh");
        database[0x13AF] = QStringLiteral("Cow bone");
        database[0x13B0] = QStringLiteral("Samurai wig");
        database[0x13B1] = QStringLiteral("Geisha wig");
        database[0x13B2] = QStringLiteral("Ninja hood");
        database[0x13B3] = QStringLiteral("Powdered wig");
        database[0x13B4] = QStringLiteral("Afro wig");
        database[0x13B5] = QStringLiteral("Regent wig");
        database[0x13B6] = QStringLiteral("Mohawk wig");
        database[0x13B7] = QStringLiteral("Pigtail");
        database[0x13B8] = QStringLiteral("Bunny hood");
        database[0x13B9] = QStringLiteral("Racing helmet");
        database[0x13BA] = QStringLiteral("Motocross helmet");
        database[0x13BB] = QStringLiteral("Football helmet");
        database[0x13BC] = QStringLiteral("Knight's helmet");
        database[0x13BD] = QStringLiteral("Roman helmet");
        database[0x13BE] = QStringLiteral("Diving mask");
        database[0x13BF] = QStringLiteral("Space helmet");
        database[0x13C0] = QStringLiteral("Ski mask");
        database[0x13C1] = QStringLiteral("Wrestling mask");
        database[0x13C2] = QStringLiteral("Dressing");
        database[0x13C3] = QStringLiteral("King Tut wig");
        database[0x13C4] = QStringLiteral("White veil");
        database[0x13C5] = QStringLiteral("White veil");
        database[0x13C6] = QStringLiteral("White veil");
        database[0x13C7] = QStringLiteral("White veil");
        database[0x13C8] = QStringLiteral("White cap");
        database[0x13C9] = QStringLiteral("Red cap");
        database[0x13CA] = QStringLiteral("Yellow cap");
        database[0x13CB] = QStringLiteral("Light blue cap");
        database[0x13CC] = QStringLiteral("Green cap");
        database[0x13CD] = QStringLiteral("Star cap");
        database[0x13CE] = QStringLiteral("1-Up cap");
        database[0x13CF] = QStringLiteral("Purple cap");
        database[0x13D0] = QStringLiteral("Blue knit hat");
        database[0x13D1] = QStringLiteral("Green knit hat");
        database[0x13D2] = QStringLiteral("Orange knit hat");
        database[0x13D3] = QStringLiteral("Pink knit hat");
        database[0x13D4] = QStringLiteral("Purple knit hat");
        database[0x13D5] = QStringLiteral("Red pom-pom hat");
        database[0x13D6] = QStringLiteral("Badge hat");
        database[0x13D7] = QStringLiteral("Straw hat");
        database[0x13D8] = QStringLiteral("Grandpa hat");
        database[0x13D9] = QStringLiteral("Paperboy cap");
        database[0x13DA] = QStringLiteral("Beret");
        database[0x13DB] = QStringLiteral("Dandy hat");
        database[0x13DC] = QStringLiteral("Hunter's cap");
        database[0x13DD] = QStringLiteral("Detective hat");
        database[0x13DE] = QStringLiteral("Top hat");
        database[0x13DF] = QStringLiteral("Puffy hat");
        database[0x13E0] = QStringLiteral("Bandana");
        database[0x13E1] = QStringLiteral("Swimming cap");
        database[0x13E2] = QStringLiteral("Wizard's cap");
        database[0x13E3] = QStringLiteral("Big bro's hat");
        database[0x13E4] = QStringLiteral("Li'l bro's hat");
        database[0x13E5] = QStringLiteral("Bicycle helmet");
        database[0x13E6] = QStringLiteral("Chef's hat");
        database[0x13E7] = QStringLiteral("Combat helmet");
        database[0x13E8] = QStringLiteral("Scholar's hat");
        database[0x13E9] = QStringLiteral("Safety helmet");
        database[0x13EA] = QStringLiteral("Police cap");
        database[0x13EB] = QStringLiteral("Explorer's hat");
        database[0x13EC] = QStringLiteral("Captain's hat");
        database[0x13ED] = QStringLiteral("Sailor's hat");
        database[0x13EE] = QStringLiteral("Silk hat");
        database[0x13EF] = QStringLiteral("Outback hat");
        database[0x13F0] = QStringLiteral("Dutch hat");
        database[0x13F1] = QStringLiteral("Sombrero");
        database[0x13F2] = QStringLiteral("Tyrolean hat");
        database[0x13F3] = QStringLiteral("Viking helmet");
        database[0x13F4] = QStringLiteral("Cavalier hat");
        database[0x13F5] = QStringLiteral("Pirate's hat");
        database[0x13F6] = QStringLiteral("Jester's cap");
        database[0x13F7] = QStringLiteral("Witch's hat");

        // ========================================
        // HAIR ACCESSORIES (0x13F8 - 0x1407)
        // ========================================
        database[0x13F8] = QStringLiteral("Red ribbon");
        database[0x13F9] = QStringLiteral("Blue ribbon");
        database[0x13FA] = QStringLiteral("Yellow ribbon");
        database[0x13FB] = QStringLiteral("Heart hairpin");
        database[0x13FC] = QStringLiteral("Star hairpin");
        database[0x13FD] = QStringLiteral("Moon hairpin");
        database[0x13FE] = QStringLiteral("Green feather");
        database[0x13FF] = QStringLiteral("Blue feather");
        database[0x1400] = QStringLiteral("Yellow feather");
        database[0x1401] = QStringLiteral("Red feather");
        database[0x1402] = QStringLiteral("Purple feather");
        database[0x1403] = QStringLiteral("White feather");
        database[0x1404] = QStringLiteral("Rainbow feather");
        database[0x1405] = QStringLiteral("Halo");
        database[0x1406] = QStringLiteral("Royal crown");
        database[0x1407] = QStringLiteral("Crown");

        // ========================================
        // FLOWERS (0x1408 - 0x148D)
        // ========================================
        database[0x1408] = QStringLiteral("Red tulips");
        database[0x1409] = QStringLiteral("White tulips");
        database[0x140A] = QStringLiteral("Yellow tulips");
        database[0x140B] = QStringLiteral("Pink tulips");
        database[0x140C] = QStringLiteral("Purple tulips");
        database[0x140D] = QStringLiteral("Black tulips");
        database[0x140E] = QStringLiteral("White pansies");
        database[0x140F] = QStringLiteral("Yellow pansies");
        database[0x1410] = QStringLiteral("Red pansies");
        database[0x1411] = QStringLiteral("Purple pansies");
        database[0x1412] = QStringLiteral("Orange pansies");
        database[0x1413] = QStringLiteral("Blue pansies");
        database[0x1414] = QStringLiteral("White cosmos");
        database[0x1415] = QStringLiteral("Red cosmos");
        database[0x1416] = QStringLiteral("Sun cosmos");
        database[0x1417] = QStringLiteral("Pink cosmos");
        database[0x1418] = QStringLiteral("Orange cosmos");
        database[0x1419] = QStringLiteral("Black cosmos");
        database[0x1422] = QStringLiteral("Jacob's Ladder");
        database[0x1425] = QStringLiteral("Dandelions");
        database[0x1428] = QStringLiteral("Four-leaf clover");
        database[0x1483] = QStringLiteral("Red roses");
        database[0x1484] = QStringLiteral("White roses");
        database[0x1485] = QStringLiteral("Yellow roses");
        database[0x1486] = QStringLiteral("Pink roses");
        database[0x1487] = QStringLiteral("Orange roses");
        database[0x1488] = QStringLiteral("Purple roses");
        database[0x1489] = QStringLiteral("Black roses");
        database[0x148A] = QStringLiteral("Blue roses");
        database[0x148D] = QStringLiteral("Gold roses");

        // ========================================
        // GLASSES/EYEWEAR (0x1431 - 0x144E)
        // ========================================
        database[0x1431] = QStringLiteral("Blue frames");
        database[0x1432] = QStringLiteral("Red glasses");
        database[0x1433] = QStringLiteral("Yellow glasses");
        database[0x1434] = QStringLiteral("Mint glasses");
        database[0x1435] = QStringLiteral("Purple glasses");
        database[0x1436] = QStringLiteral("Green glasses");
        database[0x1437] = QStringLiteral("Brown glasses");
        database[0x1438] = QStringLiteral("Pink glasses");
        database[0x1439] = QStringLiteral("Thick glasses");
        database[0x143A] = QStringLiteral("Silver frames");
        database[0x143B] = QStringLiteral("Rimmed glasses");
        database[0x143C] = QStringLiteral("Funky glasses");
        database[0x143D] = QStringLiteral("Pilot shades");
        database[0x143E] = QStringLiteral("Sporty shades");
        database[0x143F] = QStringLiteral("Huge shades");
        database[0x1440] = QStringLiteral("Triangle shades");
        database[0x1441] = QStringLiteral("Tiny shades");
        database[0x1442] = QStringLiteral("3-D glasses");
        database[0x1443] = QStringLiteral("Masquerade mask");
        database[0x1444] = QStringLiteral("Hero's mask");
        database[0x1445] = QStringLiteral("Goggles");
        database[0x1446] = QStringLiteral("Snorkel mask");
        database[0x1447] = QStringLiteral("Eye mask");
        database[0x1448] = QStringLiteral("HMD");
        database[0x1449] = QStringLiteral("Monocle");
        database[0x144A] = QStringLiteral("Doctor's mirror");
        database[0x144B] = QStringLiteral("Eye patch");
        database[0x144C] = QStringLiteral("Post-op patch");
        database[0x144D] = QStringLiteral("Gas mask");
        database[0x144E] = QStringLiteral("Hockey mask");

        // ========================================
        // FACIAL ACCESSORIES (0x144F - 0x1456)
        // Note: 0x1457-0x1470 are Mini-mustache variants
        // ========================================
        database[0x144F] = QStringLiteral("Red nose");
        database[0x1450] = QStringLiteral("Stache & glasses");
        database[0x1451] = QStringLiteral("Big bro mustache");
        database[0x1452] = QStringLiteral("Curly mustache");
        database[0x1453] = QStringLiteral("Mini-mustache");
        database[0x1454] = QStringLiteral("King's beard");
        database[0x1455] = QStringLiteral("Mask");
        database[0x1456] = QStringLiteral("Pacifier");

        // ========================================
        // BAGS OF BELLS (0x1492 - 0x14FD)
        // ========================================
        database[0x1492] = QStringLiteral("100 Bells");
        database[0x1493] = QStringLiteral("200 Bells");
        database[0x1494] = QStringLiteral("300 Bells");
        database[0x1495] = QStringLiteral("400 Bells");
        database[0x1496] = QStringLiteral("500 Bells");
        database[0x1497] = QStringLiteral("600 Bells");
        database[0x1498] = QStringLiteral("700 Bells");
        database[0x1499] = QStringLiteral("800 Bells");
        database[0x149A] = QStringLiteral("900 Bells");
        database[0x149B] = QStringLiteral("1,000 Bells");
        database[0x149C] = QStringLiteral("2,000 Bells");
        database[0x149D] = QStringLiteral("3,000 Bells");
        database[0x149E] = QStringLiteral("4,000 Bells");
        database[0x149F] = QStringLiteral("5,000 Bells");
        database[0x14A0] = QStringLiteral("6,000 Bells");
        database[0x14A1] = QStringLiteral("7,000 Bells");
        database[0x14A2] = QStringLiteral("8,000 Bells");
        database[0x14A3] = QStringLiteral("9,000 Bells");
        database[0x14A4] = QStringLiteral("10,000 Bells");
        database[0x14A5] = QStringLiteral("11,000 Bells");
        database[0x14A6] = QStringLiteral("12,000 Bells");
        database[0x14A7] = QStringLiteral("13,000 Bells");
        database[0x14A8] = QStringLiteral("14,000 Bells");
        database[0x14A9] = QStringLiteral("15,000 Bells");
        database[0x14AA] = QStringLiteral("16,000 Bells");
        database[0x14AB] = QStringLiteral("17,000 Bells");
        database[0x14AC] = QStringLiteral("18,000 Bells");
        database[0x14AD] = QStringLiteral("19,000 Bells");
        database[0x14AE] = QStringLiteral("20,000 Bells");
        database[0x14AF] = QStringLiteral("21,000 Bells");
        database[0x14B0] = QStringLiteral("22,000 Bells");
        database[0x14B1] = QStringLiteral("23,000 Bells");
        database[0x14B2] = QStringLiteral("24,000 Bells");
        database[0x14B3] = QStringLiteral("25,000 Bells");
        database[0x14B4] = QStringLiteral("26,000 Bells");
        database[0x14B5] = QStringLiteral("27,000 Bells");
        database[0x14B6] = QStringLiteral("28,000 Bells");
        database[0x14B7] = QStringLiteral("29,000 Bells");
        database[0x14B8] = QStringLiteral("30,000 Bells");
        database[0x14B9] = QStringLiteral("31,000 Bells");
        database[0x14BA] = QStringLiteral("32,000 Bells");
        database[0x14BB] = QStringLiteral("33,000 Bells");
        database[0x14BC] = QStringLiteral("34,000 Bells");
        database[0x14BD] = QStringLiteral("35,000 Bells");
        database[0x14BE] = QStringLiteral("36,000 Bells");
        database[0x14BF] = QStringLiteral("37,000 Bells");
        database[0x14C0] = QStringLiteral("38,000 Bells");
        database[0x14C1] = QStringLiteral("39,000 Bells");
        database[0x14C2] = QStringLiteral("40,000 Bells");
        database[0x14C3] = QStringLiteral("41,000 Bells");
        database[0x14C4] = QStringLiteral("42,000 Bells");
        database[0x14C5] = QStringLiteral("43,000 Bells");
        database[0x14C6] = QStringLiteral("44,000 Bells");
        database[0x14C7] = QStringLiteral("45,000 Bells");
        database[0x14C8] = QStringLiteral("46,000 Bells");
        database[0x14C9] = QStringLiteral("47,000 Bells");
        database[0x14CA] = QStringLiteral("48,000 Bells");
        database[0x14CB] = QStringLiteral("49,000 Bells");
        database[0x14CC] = QStringLiteral("50,000 Bells");
        database[0x14CD] = QStringLiteral("51,000 Bells");
        database[0x14CE] = QStringLiteral("52,000 Bells");
        database[0x14CF] = QStringLiteral("53,000 Bells");
        database[0x14D0] = QStringLiteral("54,000 Bells");
        database[0x14D1] = QStringLiteral("55,000 Bells");
        database[0x14D2] = QStringLiteral("56,000 Bells");
        database[0x14D3] = QStringLiteral("57,000 Bells");
        database[0x14D4] = QStringLiteral("58,000 Bells");
        database[0x14D5] = QStringLiteral("59,000 Bells");
        database[0x14D6] = QStringLiteral("60,000 Bells");
        database[0x14D7] = QStringLiteral("61,000 Bells");
        database[0x14D8] = QStringLiteral("62,000 Bells");
        database[0x14D9] = QStringLiteral("63,000 Bells");
        database[0x14DA] = QStringLiteral("64,000 Bells");
        database[0x14DB] = QStringLiteral("65,000 Bells");
        database[0x14DC] = QStringLiteral("66,000 Bells");
        database[0x14DD] = QStringLiteral("67,000 Bells");
        database[0x14DE] = QStringLiteral("68,000 Bells");
        database[0x14DF] = QStringLiteral("69,000 Bells");
        database[0x14E0] = QStringLiteral("70,000 Bells");
        database[0x14E1] = QStringLiteral("71,000 Bells");
        database[0x14E2] = QStringLiteral("72,000 Bells");
        database[0x14E3] = QStringLiteral("73,000 Bells");
        database[0x14E4] = QStringLiteral("74,000 Bells");
        database[0x14E5] = QStringLiteral("75,000 Bells");
        database[0x14E6] = QStringLiteral("76,000 Bells");
        database[0x14E7] = QStringLiteral("77,000 Bells");
        database[0x14E8] = QStringLiteral("78,000 Bells");
        database[0x14E9] = QStringLiteral("79,000 Bells");
        database[0x14EA] = QStringLiteral("80,000 Bells");
        database[0x14EB] = QStringLiteral("81,000 Bells");
        database[0x14EC] = QStringLiteral("82,000 Bells");
        database[0x14ED] = QStringLiteral("83,000 Bells");
        database[0x14EE] = QStringLiteral("84,000 Bells");
        database[0x14EF] = QStringLiteral("85,000 Bells");
        database[0x14F0] = QStringLiteral("86,000 Bells");
        database[0x14F1] = QStringLiteral("87,000 Bells");
        database[0x14F2] = QStringLiteral("88,000 Bells");
        database[0x14F3] = QStringLiteral("89,000 Bells");
        database[0x14F4] = QStringLiteral("90,000 Bells");
        database[0x14F5] = QStringLiteral("91,000 Bells");
        database[0x14F6] = QStringLiteral("92,000 Bells");
        database[0x14F7] = QStringLiteral("93,000 Bells");
        database[0x14F8] = QStringLiteral("94,000 Bells");
        database[0x14F9] = QStringLiteral("95,000 Bells");
        database[0x14FA] = QStringLiteral("96,000 Bells");
        database[0x14FB] = QStringLiteral("97,000 Bells");
        database[0x14FC] = QStringLiteral("98,000 Bells");
        database[0x14FD] = QStringLiteral("99,000 Bells");

        // ========================================
        // SEED BAGS (0x14FE - 0x1512)
        // ========================================
        database[0x14FE] = QStringLiteral("Red tulip bag");
        database[0x14FF] = QStringLiteral("White tulip bag");
        database[0x1500] = QStringLiteral("Yellow tulip bag");
        database[0x1504] = QStringLiteral("White pansy bag");
        database[0x1505] = QStringLiteral("Yellow pansy bag");
        database[0x1506] = QStringLiteral("Red pansy bag");
        database[0x150A] = QStringLiteral("White cosmos bag");
        database[0x150B] = QStringLiteral("Red cosmos bag");
        database[0x150C] = QStringLiteral("Sun cosmos bag");
        database[0x1510] = QStringLiteral("Red rose bag");
        database[0x1511] = QStringLiteral("White rose bag");
        database[0x1512] = QStringLiteral("Yellow rose bag");

        // ========================================
        // FRUIT & SAPLINGS (0x1518 - 0x1548)
        // ========================================
        database[0x1518] = QStringLiteral("Apple");
        database[0x1519] = QStringLiteral("Orange");
        database[0x151A] = QStringLiteral("Pear");
        database[0x151B] = QStringLiteral("Peach");
        database[0x151C] = QStringLiteral("Cherry");
        database[0x151D] = QStringLiteral("Sapling");
        database[0x151E] = QStringLiteral("Cedar sapling");
        database[0x151F] = QStringLiteral("Note in a bottle");
        database[0x1520] = QStringLiteral("Note in a bottle");
        database[0x1548] = QStringLiteral("Coconut");

        // ========================================
        // PAINT (0x1521 - 0x1530)
        // ========================================
        database[0x1521] = QStringLiteral("Gray paint");
        database[0x1522] = QStringLiteral("Brown paint");
        database[0x1523] = QStringLiteral("Red paint");
        database[0x1524] = QStringLiteral("Pink paint");
        database[0x1525] = QStringLiteral("Orange paint");
        database[0x1526] = QStringLiteral("Yellow paint");
        database[0x1527] = QStringLiteral("Pale green paint");
        database[0x1528] = QStringLiteral("Green paint");
        database[0x1529] = QStringLiteral("Dark green paint");
        database[0x152A] = QStringLiteral("Turquoise paint");
        database[0x152B] = QStringLiteral("Sky blue paint");
        database[0x152C] = QStringLiteral("Blue paint");
        database[0x152D] = QStringLiteral("Navy paint");
        database[0x152E] = QStringLiteral("Purple paint");
        database[0x152F] = QStringLiteral("Lilac paint");
        database[0x1530] = QStringLiteral("Hot pink paint");

        // ========================================
        // TURNIPS (0x1531 - 0x153A, 0x153B - 0x1541, 0x154A - 0x1553, 0x1567)
        // ========================================
        database[0x1531] = QStringLiteral("10 turnips");
        database[0x1532] = QStringLiteral("20 turnips");
        database[0x1533] = QStringLiteral("30 turnips");
        database[0x1534] = QStringLiteral("40 turnips");
        database[0x1535] = QStringLiteral("50 turnips");
        database[0x1536] = QStringLiteral("60 turnips");
        database[0x1537] = QStringLiteral("70 turnips");
        database[0x1538] = QStringLiteral("80 turnips");
        database[0x1539] = QStringLiteral("90 turnips");
        database[0x153A] = QStringLiteral("100 turnips");
        database[0x153B] = QStringLiteral("Red turnip");
        database[0x153C] = QStringLiteral("Red turnip");
        database[0x153D] = QStringLiteral("Red turnip");
        database[0x153E] = QStringLiteral("Red turnip");
        database[0x153F] = QStringLiteral("Red turnip");
        database[0x1540] = QStringLiteral("Red turnip");
        database[0x1541] = QStringLiteral("Red turnip");
        database[0x154A] = QStringLiteral("Spoiled turnips");
        database[0x154B] = QStringLiteral("Spoiled turnips");
        database[0x154C] = QStringLiteral("Spoiled turnips");
        database[0x154D] = QStringLiteral("Spoiled turnips");
        database[0x154E] = QStringLiteral("Spoiled turnips");
        database[0x154F] = QStringLiteral("Spoiled turnips");
        database[0x1550] = QStringLiteral("Spoiled turnips");
        database[0x1551] = QStringLiteral("Spoiled turnips");
        database[0x1552] = QStringLiteral("Spoiled turnips");
        database[0x1553] = QStringLiteral("Spoiled turnips");
        database[0x1567] = QStringLiteral("Red turnip seed");

        // ========================================
        // ACORNS (0x1542 - 0x1546)
        // ========================================
        database[0x1542] = QStringLiteral("Acorn");
        database[0x1543] = QStringLiteral("Large acorn");
        database[0x1544] = QStringLiteral("Small acorn");
        database[0x1545] = QStringLiteral("Round acorn");
        database[0x1546] = QStringLiteral("Rotten acorn");

        // ========================================
        // SHELLS (0x1554 - 0x155C)
        // ========================================
        database[0x1554] = QStringLiteral("Pearl oyster");
        database[0x1555] = QStringLiteral("Conch");
        database[0x1556] = QStringLiteral("White scallop");
        database[0x1557] = QStringLiteral("Coral");
        database[0x1558] = QStringLiteral("Venus comb");
        database[0x1559] = QStringLiteral("Scallop");
        database[0x155A] = QStringLiteral("Dall's Top");
        database[0x155B] = QStringLiteral("Porceletta");
        database[0x155C] = QStringLiteral("Sand dollar");

        // ========================================
        // MISCELLANEOUS ITEMS (0x1547 - 0x156D)
        // ========================================
        database[0x1547] = QStringLiteral("Sold-out sign");
        database[0x1549] = QStringLiteral("Fossil");
        database[0x155D] = QStringLiteral("Lost item");
        database[0x155E] = QStringLiteral("Medicine");
        database[0x155F] = QStringLiteral("Carpet package");
        database[0x1560] = QStringLiteral("Wall package");
        database[0x1561] = QStringLiteral("S carpet package");
        database[0x1562] = QStringLiteral("S wall package");
        database[0x1563] = QStringLiteral("S furniture box");
        database[0x1564] = QStringLiteral("S can package");
        database[0x1565] = QStringLiteral("Letter");
        database[0x1566] = QStringLiteral("Pitfall seed");
        database[0x1568] = QStringLiteral("UFO part");
        database[0x156B] = QStringLiteral("Present");
        database[0x156C] = QStringLiteral("Timer");
        database[0x156D] = QStringLiteral("Garbage");

        // ========================================
        // FURNITURE - EXOTIC SERIES (0x3000 - 0x3024)
        // Note: Furniture uses every 4th hex value
        // ========================================
        database[0x3000] = QStringLiteral("Exotic bed");
        database[0x3004] = QStringLiteral("Exotic bureau");
        database[0x3008] = QStringLiteral("Exotic wardrobe");
        database[0x300C] = QStringLiteral("Exotic bench");
        database[0x3010] = QStringLiteral("Exotic chair");
        database[0x3014] = QStringLiteral("Exotic table");
        database[0x3018] = QStringLiteral("Exotic end table");
        database[0x301C] = QStringLiteral("Exotic lamp");
        database[0x3020] = QStringLiteral("Exotic screen");
        database[0x3024] = QStringLiteral("Exotic chest");

        // ========================================
        // FURNITURE - RANCH SERIES (0x3028 - 0x304C)
        // ========================================
        database[0x3028] = QStringLiteral("Ranch bed");
        database[0x302C] = QStringLiteral("Ranch dresser");
        database[0x3030] = QStringLiteral("Ranch wardrobe");
        database[0x3034] = QStringLiteral("Ranch couch");
        database[0x3038] = QStringLiteral("Ranch armchair");
        database[0x303C] = QStringLiteral("Ranch chair");
        database[0x3040] = QStringLiteral("Ranch table");
        database[0x3044] = QStringLiteral("Ranch tea table");
        database[0x3048] = QStringLiteral("Ranch bookcase");
        database[0x304C] = QStringLiteral("Ranch hutch");

        // ========================================
        // FURNITURE - CLASSIC SERIES (0x3050 - 0x3074)
        // ========================================
        database[0x3050] = QStringLiteral("Classic bed");
        database[0x3054] = QStringLiteral("Classic vanity");
        database[0x3058] = QStringLiteral("Classic wardrobe");
        database[0x305C] = QStringLiteral("Classic chair");
        database[0x3060] = QStringLiteral("Classic sofa");
        database[0x3064] = QStringLiteral("Classic table");
        database[0x3068] = QStringLiteral("Classic bookcase");
        database[0x306C] = QStringLiteral("Classic buffet");
        database[0x3070] = QStringLiteral("Classic desk");
        database[0x3074] = QStringLiteral("Classic clock");

        // ========================================
        // FURNITURE - REGAL SERIES (0x3078 - 0x309C)
        // ========================================
        database[0x3078] = QStringLiteral("Regal bed");
        database[0x307C] = QStringLiteral("Regal dresser");
        database[0x3080] = QStringLiteral("Regal armoire");
        database[0x3084] = QStringLiteral("Regal chair");
        database[0x3088] = QStringLiteral("Regal sofa");
        database[0x308C] = QStringLiteral("Regal table");
        database[0x3090] = QStringLiteral("Regal lamp");
        database[0x3094] = QStringLiteral("Regal clock");
        database[0x3098] = QStringLiteral("Regal bookcase");
        database[0x309C] = QStringLiteral("Regal vanity");

        // ========================================
        // FURNITURE - BLUE SERIES (0x30A0 - 0x30C4)
        // ========================================
        database[0x30A0] = QStringLiteral("Blue bed");
        database[0x30A4] = QStringLiteral("Blue bureau");
        database[0x30A8] = QStringLiteral("Blue wardrobe");
        database[0x30AC] = QStringLiteral("Blue chair");
        database[0x30B0] = QStringLiteral("Blue bench");
        database[0x30B4] = QStringLiteral("Blue table");
        database[0x30B8] = QStringLiteral("Blue bookcase");
        database[0x30BC] = QStringLiteral("Blue cabinet");
        database[0x30C0] = QStringLiteral("Blue clock");
        database[0x30C4] = QStringLiteral("Blue dresser");

        // ========================================
        // FURNITURE - CABANA SERIES (0x30C8 - 0x30EC)
        // ========================================
        database[0x30C8] = QStringLiteral("Cabana bed");
        database[0x30CC] = QStringLiteral("Cabana dresser");
        database[0x30D0] = QStringLiteral("Cabana wardrobe");
        database[0x30D4] = QStringLiteral("Cabana chair");
        database[0x30D8] = QStringLiteral("Cabana armchair");
        database[0x30DC] = QStringLiteral("Cabana table");
        database[0x30E0] = QStringLiteral("Cabana lamp");
        database[0x30E4] = QStringLiteral("Cabana bookcase");
        database[0x30E8] = QStringLiteral("Cabana vanity");
        database[0x30EC] = QStringLiteral("Cabana screen");

        // ========================================
        // FURNITURE - GREEN SERIES (0x30F0 - 0x3114)
        // ========================================
        database[0x30F0] = QStringLiteral("Green bed");
        database[0x30F4] = QStringLiteral("Green dresser");
        database[0x30F8] = QStringLiteral("Green wardrobe");
        database[0x30FC] = QStringLiteral("Green chair");
        database[0x3100] = QStringLiteral("Green bench");
        database[0x3104] = QStringLiteral("Green table");
        database[0x3108] = QStringLiteral("Green desk");
        database[0x310C] = QStringLiteral("Green lamp");
        database[0x3110] = QStringLiteral("Green counter");
        database[0x3114] = QStringLiteral("Green pantry");

        // ========================================
        // FURNITURE - CABIN SERIES (0x3118 - 0x313C)
        // ========================================
        database[0x3118] = QStringLiteral("Cabin bed");
        database[0x311C] = QStringLiteral("Cabin dresser");
        database[0x3120] = QStringLiteral("Cabin wardrobe");
        database[0x3124] = QStringLiteral("Cabin chair");
        database[0x3128] = QStringLiteral("Cabin armchair");
        database[0x312C] = QStringLiteral("Cabin couch");
        database[0x3130] = QStringLiteral("Cabin table");
        database[0x3134] = QStringLiteral("Cabin low table");
        database[0x3138] = QStringLiteral("Cabin bookcase");
        database[0x313C] = QStringLiteral("Cabin clock");

        // ========================================
        // FURNITURE - MODERN SERIES (0x3140 - 0x3164)
        // ========================================
        database[0x3140] = QStringLiteral("Modern bed");
        database[0x3144] = QStringLiteral("Modern dresser");
        database[0x3148] = QStringLiteral("Modern wardrobe");
        database[0x314C] = QStringLiteral("Modern chair");
        database[0x3150] = QStringLiteral("Modern sofa");
        database[0x3154] = QStringLiteral("Modern end table");
        database[0x3158] = QStringLiteral("Modern desk");
        database[0x315C] = QStringLiteral("Modern table");
        database[0x3160] = QStringLiteral("Modern cabinet");
        database[0x3164] = QStringLiteral("Modern lamp");

        // ========================================
        // FURNITURE - KIDDIE SERIES (0x3168 - 0x318C)
        // ========================================
        database[0x3168] = QStringLiteral("Kiddie bed");
        database[0x316C] = QStringLiteral("Kiddie dresser");
        database[0x3170] = QStringLiteral("Kiddie wardrobe");
        database[0x3174] = QStringLiteral("Kiddie couch");
        database[0x3178] = QStringLiteral("Kiddie chair");
        database[0x317C] = QStringLiteral("Kiddie table");
        database[0x3180] = QStringLiteral("Kiddie bureau");
        database[0x3184] = QStringLiteral("Kiddie bookcase");
        database[0x3188] = QStringLiteral("Kiddie clock");
        database[0x318C] = QStringLiteral("Kiddie stereo");

        // ========================================
        // FURNITURE - LOVELY SERIES (0x3190 - 0x31B4)
        // ========================================
        database[0x3190] = QStringLiteral("Lovely bed");
        database[0x3194] = QStringLiteral("Lovely dresser");
        database[0x3198] = QStringLiteral("Lovely armoire");
        database[0x319C] = QStringLiteral("Lovely chair");
        database[0x31A0] = QStringLiteral("Lovely loveseat");
        database[0x31A4] = QStringLiteral("Lovely end table");
        database[0x31A8] = QStringLiteral("Lovely table");
        database[0x31AC] = QStringLiteral("Lovely vanity");
        database[0x31B0] = QStringLiteral("Lovely kitchen");
        database[0x31B4] = QStringLiteral("Lovely lamp");

        // ========================================
        // FURNITURE - ROBO SERIES (0x31B8 - 0x31DC)
        // ========================================
        database[0x31B8] = QStringLiteral("Robo-bed");
        database[0x31BC] = QStringLiteral("Robo-dresser");
        database[0x31C0] = QStringLiteral("Robo-closet");
        database[0x31C4] = QStringLiteral("Robo-sofa");
        database[0x31C8] = QStringLiteral("Robo-chair");
        database[0x31CC] = QStringLiteral("Robo-table");
        database[0x31D0] = QStringLiteral("Robo-stereo");
        database[0x31D4] = QStringLiteral("Robo-TV");
        database[0x31D8] = QStringLiteral("Robo-clock");
        database[0x31DC] = QStringLiteral("Robo-lamp");

        // ========================================
        // FURNITURE - SNOWMAN SERIES (0x31E0 - 0x3204)
        // ========================================
        database[0x31E0] = QStringLiteral("Snowman bed");
        database[0x31E4] = QStringLiteral("Snowman dresser");
        database[0x31E8] = QStringLiteral("Snowman wardrobe");
        database[0x31EC] = QStringLiteral("Snowman chair");
        database[0x31F0] = QStringLiteral("Snowman sofa");
        database[0x31F4] = QStringLiteral("Snowman table");
        database[0x31F8] = QStringLiteral("Snowman fridge");
        database[0x31FC] = QStringLiteral("Snowman TV");
        database[0x3200] = QStringLiteral("Snowman clock");
        database[0x3204] = QStringLiteral("Snowman lamp");

        // ========================================
        // FURNITURE - MUSH SERIES (0x3208 - 0x322C)
        // ========================================
        database[0x3208] = QStringLiteral("Mush bed");
        database[0x320C] = QStringLiteral("Mush dresser");
        database[0x3210] = QStringLiteral("Mush closet");
        database[0x3214] = QStringLiteral("Mush chair");
        database[0x3218] = QStringLiteral("Mush stool");
        database[0x321C] = QStringLiteral("Mush table");
        database[0x3220] = QStringLiteral("Mush end table");
        database[0x3224] = QStringLiteral("Mush lamp");
        database[0x3228] = QStringLiteral("Mush stand");
        database[0x322C] = QStringLiteral("Mush TV");

        // ========================================
        // FURNITURE - WESTERN THEME (0x3230 - 0x3254)
        // ========================================
        database[0x3230] = QStringLiteral("Cow skull");
        database[0x3234] = QStringLiteral("Desert cactus");
        database[0x3238] = QStringLiteral("Western fence");
        database[0x323C] = QStringLiteral("Saddle fence");
        database[0x3240] = QStringLiteral("Storefront");
        database[0x3244] = QStringLiteral("Covered wagon");
        database[0x3248] = QStringLiteral("Watering trough");
        database[0x324C] = QStringLiteral("Tumbleweed");
        database[0x3250] = QStringLiteral("Well");
        database[0x3254] = QStringLiteral("Wagon wheel");

        // ========================================
        // FURNITURE - SPACE THEME (0x3258 - 0x327C)
        // ========================================
        database[0x3258] = QStringLiteral("Asteroid");
        database[0x325C] = QStringLiteral("Moon");
        database[0x3260] = QStringLiteral("Lunar lander");
        database[0x3264] = QStringLiteral("Rocket");
        database[0x3268] = QStringLiteral("Lunar rover");
        database[0x326C] = QStringLiteral("Satellite");
        database[0x3270] = QStringLiteral("Space shuttle");
        database[0x3274] = QStringLiteral("Spaceman Sam");
        database[0x3278] = QStringLiteral("Space station");
        database[0x327C] = QStringLiteral("Flying saucer");

        // ========================================
        // FURNITURE - CONSTRUCTION THEME (0x3280 - 0x32AC)
        // ========================================
        database[0x3280] = QStringLiteral("Detour arrow");
        database[0x3284] = QStringLiteral("Handcart");
        database[0x3288] = QStringLiteral("Iron frame");
        database[0x328C] = QStringLiteral("Manhole cover");
        database[0x3290] = QStringLiteral("Orange cone");
        database[0x3294] = QStringLiteral("Traffic cone");
        database[0x3298] = QStringLiteral("Saw horse");
        database[0x329C] = QStringLiteral("Wet roadway sign");
        database[0x32A0] = QStringLiteral("Steam roller");
        database[0x32A4] = QStringLiteral("Jackhammer");
        database[0x32A8] = QStringLiteral("Striped cone");
        database[0x32AC] = QStringLiteral("Cement mixer");

        // ========================================
        // FURNITURE - DRUMS & BARRELS (0x32B0 - 0x32C0)
        // ========================================
        database[0x32B0] = QStringLiteral("Brown drum");
        database[0x32B4] = QStringLiteral("Green drum");
        database[0x32B8] = QStringLiteral("Red drum");
        database[0x32BC] = QStringLiteral("Haz-mat barrel");
        database[0x32C0] = QStringLiteral("Oil drum");

        // ========================================
        // FURNITURE - BOXING THEME (0x32C4 - 0x32E8)
        // ========================================
        database[0x32C4] = QStringLiteral("Boxing barricade");
        database[0x32C8] = QStringLiteral("Red corner");
        database[0x32CC] = QStringLiteral("Blue corner");
        database[0x32D0] = QStringLiteral("Neutral corner");
        database[0x32D4] = QStringLiteral("Judge's bell");
        database[0x32D8] = QStringLiteral("Boxing mat");
        database[0x32DC] = QStringLiteral("Ringside table");
        database[0x32E0] = QStringLiteral("Weight bench");
        database[0x32E4] = QStringLiteral("Sandbag");
        database[0x32E8] = QStringLiteral("Speed bag");

        // ========================================
        // FURNITURE - BACKYARD THEME (0x32EC - 0x3328)
        // ========================================
        database[0x32EC] = QStringLiteral("Birdhouse");
        database[0x32F0] = QStringLiteral("Mr. Flamingo");
        database[0x32F4] = QStringLiteral("Mrs. Flamingo");
        database[0x32F8] = QStringLiteral("Garden gnome");
        database[0x32FC] = QStringLiteral("Bug zapper");
        database[0x3300] = QStringLiteral("Hammock");
        database[0x3304] = QStringLiteral("Bird bath");
        database[0x3308] = QStringLiteral("Picnic table");
        database[0x330C] = QStringLiteral("Bird feeder");
        database[0x3310] = QStringLiteral("Lawn chair");
        database[0x3314] = QStringLiteral("Lawn mower");
        database[0x3318] = QStringLiteral("Barbecue");
        database[0x331C] = QStringLiteral("Backyard pool");
        database[0x3320] = QStringLiteral("Sprinkler");
        database[0x3324] = QStringLiteral("Tiki torch");
        database[0x3328] = QStringLiteral("Garbage");

        // ========================================
        // FURNITURE - PLAYROOM THEME (0x332C - 0x3350)
        // ========================================
        database[0x332C] = QStringLiteral("Elephant slide");
        database[0x3330] = QStringLiteral("Stroller");
        database[0x3334] = QStringLiteral("Cradle");
        database[0x3338] = QStringLiteral("Baby bed");
        database[0x333C] = QStringLiteral("Clackercart");
        database[0x3340] = QStringLiteral("Merry-go-round");
        database[0x3344] = QStringLiteral("Rocking horse");
        database[0x3348] = QStringLiteral("Train set");
        database[0x334C] = QStringLiteral("Wobbelina");
        database[0x3350] = QStringLiteral("Dolly");

        // ========================================
        // FURNITURE - LAB THEME (0x3354 - 0x336C)
        // ========================================
        database[0x3354] = QStringLiteral("Lab bench");
        database[0x3358] = QStringLiteral("Lab chair");
        database[0x335C] = QStringLiteral("Medicine chest");
        database[0x3360] = QStringLiteral("Florence flask");
        database[0x3364] = QStringLiteral("Amazing machine");
        database[0x3368] = QStringLiteral("Unknown machine");
        database[0x336C] = QStringLiteral("Complex machine");

        // ========================================
        // FURNITURE - PIRATE THEME (0x3370 - 0x3384)
        // ========================================
        database[0x3370] = QStringLiteral("Helm");
        database[0x3374] = QStringLiteral("Barrel");
        database[0x3378] = QStringLiteral("Keg");
        database[0x337C] = QStringLiteral("Ship cannon");
        database[0x3380] = QStringLiteral("Anchor");
        database[0x3384] = QStringLiteral("Ship compass");

        // ========================================
        // FURNITURE - CHESS PIECES (0x3388 - 0x33B4)
        // ========================================
        database[0x3388] = QStringLiteral("Black pawn");
        database[0x338C] = QStringLiteral("Black bishop");
        database[0x3390] = QStringLiteral("Black king");
        database[0x3394] = QStringLiteral("Black knight");
        database[0x3398] = QStringLiteral("Black queen");
        database[0x339C] = QStringLiteral("Black rook");
        database[0x33A0] = QStringLiteral("White pawn");
        database[0x33A4] = QStringLiteral("White bishop");
        database[0x33A8] = QStringLiteral("White king");
        database[0x33AC] = QStringLiteral("White knight");
        database[0x33B0] = QStringLiteral("White queen");
        database[0x33B4] = QStringLiteral("White rook");

        // ========================================
        // FURNITURE - MEDICAL/SCHOOL THEME (0x33B8 - 0x33DC)
        // ========================================
        database[0x33B8] = QStringLiteral("Hamster cage");
        database[0x33BC] = QStringLiteral("Mop");
        database[0x33C0] = QStringLiteral("Skeleton");
        database[0x33C4] = QStringLiteral("Anatomical model");
        database[0x33C8] = QStringLiteral("Clear model");
        database[0x33CC] = QStringLiteral("Vision tester");
        database[0x33D0] = QStringLiteral("Trunk measure");
        database[0x33D4] = QStringLiteral("Stadiometer");
        database[0x33D8] = QStringLiteral("Scale");
        database[0x33DC] = QStringLiteral("Hospital bed");

        // ========================================
        // FURNITURE - OFFICE THEME (0x33E0 - 0x3408)
        // ========================================
        database[0x33E0] = QStringLiteral("Writing desk");
        database[0x33E4] = QStringLiteral("Writing chair");
        database[0x33E8] = QStringLiteral("Globe");
        database[0x33EC] = QStringLiteral("Office desk");
        database[0x33F0] = QStringLiteral("Office chair");
        database[0x33F4] = QStringLiteral("Office locker");
        database[0x33F8] = QStringLiteral("Coffee maker");
        database[0x33FC] = QStringLiteral("Checkout counter");
        database[0x3400] = QStringLiteral("Cash register");
        database[0x3404] = QStringLiteral("Jukebox");
        database[0x3408] = QStringLiteral("Siphon");

        // ========================================
        // FURNITURE - DHARMA & BEARS (0x340C - 0x342C)
        // ========================================
        database[0x340C] = QStringLiteral("Giant dharma");
        database[0x3410] = QStringLiteral("Dharma");
        database[0x3414] = QStringLiteral("Mini-dharma");
        database[0x3418] = QStringLiteral("Papa bear");
        database[0x341C] = QStringLiteral("Mama bear");
        database[0x3420] = QStringLiteral("Baby bear");
        database[0x3424] = QStringLiteral("Papa panda");
        database[0x3428] = QStringLiteral("Mama panda");
        database[0x342C] = QStringLiteral("Baby panda");

        // ========================================
        // FURNITURE - CACTI & GOLF BAGS (0x3430 - 0x3444)
        // ========================================
        database[0x3430] = QStringLiteral("Tall cactus");
        database[0x3434] = QStringLiteral("Cactus");
        database[0x3438] = QStringLiteral("Round cactus");
        database[0x343C] = QStringLiteral("Blue golf bag");
        database[0x3440] = QStringLiteral("White golf bag");
        database[0x3444] = QStringLiteral("Green golf bag");

        // ========================================
        // FURNITURE - MISC SEATING & TOTEM POLES (0x3448 - 0x345C)
        // ========================================
        database[0x3448] = QStringLiteral("Red armchair");
        database[0x344C] = QStringLiteral("Red sofa");
        database[0x3450] = QStringLiteral("Bear pole");
        database[0x3454] = QStringLiteral("Eagle pole");
        database[0x3458] = QStringLiteral("Frog woman pole");
        database[0x345C] = QStringLiteral("Raven pole");

        // ========================================
        // FURNITURE - LAVA LAMPS & JAPANESE (0x3460 - 0x3490)
        // ========================================
        database[0x3460] = QStringLiteral("Blue lava lamp");
        database[0x3464] = QStringLiteral("Purple lava lamp");
        database[0x3468] = QStringLiteral("Green lava lamp");
        database[0x346C] = QStringLiteral("Samurai suit");
        database[0x3470] = QStringLiteral("Black katana");
        database[0x3474] = QStringLiteral("White katana");
        database[0x3478] = QStringLiteral("Ninja sword");
        database[0x347C] = QStringLiteral("Sword");
        database[0x3480] = QStringLiteral("Bow");
        database[0x3484] = QStringLiteral("Tea table");
        database[0x3488] = QStringLiteral("Hearth");
        database[0x348C] = QStringLiteral("Deer scare");
        database[0x3490] = QStringLiteral("Tall lantern");

        // ========================================
        // FURNITURE - NINTENDO ITEMS (0x3494 - 0x34A0)
        // ========================================
        database[0x3494] = QStringLiteral("Master Sword");
        database[0x3498] = QStringLiteral("Arwing");
        database[0x349C] = QStringLiteral("Triforce");
        database[0x34A0] = QStringLiteral("Metroid");

        // ========================================
        // FURNITURE - VASES & LUCKY CATS (0x34A4 - 0x34BC)
        // ========================================
        database[0x34A4] = QStringLiteral("Tea vase");
        database[0x34A8] = QStringLiteral("Red vase");
        database[0x34AC] = QStringLiteral("Blue vase");
        database[0x34B0] = QStringLiteral("Lucky cat");
        database[0x34B4] = QStringLiteral("Lefty lucky cat");
        database[0x34B8] = QStringLiteral("Lucky black cat");
        database[0x34BC] = QStringLiteral("Lucky gold cat");

        // ========================================
        // FURNITURE - FRUIT SERIES (0x34C0 - 0x34E8)
        // ========================================
        database[0x34C0] = QStringLiteral("Grapefruit table");
        database[0x34C4] = QStringLiteral("Lemon table");
        database[0x34C8] = QStringLiteral("Orange chair");
        database[0x34CC] = QStringLiteral("Lime chair");
        database[0x34D0] = QStringLiteral("Pear wardrobe");
        database[0x34D4] = QStringLiteral("Pear dresser");
        database[0x34D8] = QStringLiteral("Watermelon chair");
        database[0x34DC] = QStringLiteral("Melon chair");
        database[0x34E0] = QStringLiteral("Watermelon table");
        database[0x34E4] = QStringLiteral("Apple TV");
        database[0x34E8] = QStringLiteral("Apple clock");

        // ========================================
        // FURNITURE - GARDEN SERIES (0x34EC - 0x3510)
        // ========================================
        database[0x34EC] = QStringLiteral("Lily-pad table");
        database[0x34F0] = QStringLiteral("Froggy chair");
        database[0x34F4] = QStringLiteral("Pine chair");
        database[0x34F8] = QStringLiteral("Pine table");
        database[0x34FC] = QStringLiteral("Tulip chair");
        database[0x3500] = QStringLiteral("Tulip table");
        database[0x3504] = QStringLiteral("Iris chair");
        database[0x3508] = QStringLiteral("Iris table");
        database[0x350C] = QStringLiteral("Daffodil chair");
        database[0x3510] = QStringLiteral("Daffodil table");

        // ========================================
        // FURNITURE - MUSICAL INSTRUMENTS (0x3514 - 0x3558)
        // ========================================
        database[0x3514] = QStringLiteral("Conga drum");
        database[0x3518] = QStringLiteral("Djimbe drum");
        database[0x351C] = QStringLiteral("Timpano drum");
        database[0x3520] = QStringLiteral("Violin");
        database[0x3524] = QStringLiteral("Cello");
        database[0x3528] = QStringLiteral("Bass");
        database[0x352C] = QStringLiteral("Folk guitar");
        database[0x3530] = QStringLiteral("Country guitar");
        database[0x3534] = QStringLiteral("Rock guitar");
        database[0x3538] = QStringLiteral("Metal guitar");
        database[0x353C] = QStringLiteral("Ukulele");
        database[0x3540] = QStringLiteral("Ebony piano");
        database[0x3544] = QStringLiteral("Ivory piano");
        database[0x3548] = QStringLiteral("Japanese drum");
        database[0x354C] = QStringLiteral("Vibraphone");
        database[0x3550] = QStringLiteral("Biwa lute");
        database[0x3554] = QStringLiteral("Harp");
        database[0x3558] = QStringLiteral("Metronome");

        // ========================================
        // FURNITURE - TRASH CANS (0x355C - 0x3568)
        // ========================================
        database[0x355C] = QStringLiteral("Trash can");
        database[0x3560] = QStringLiteral("Garbage pail");
        database[0x3564] = QStringLiteral("Garbage can");
        database[0x3568] = QStringLiteral("Trash bin");

        // ========================================
        // FURNITURE - ANCIENT/EGYPTIAN THEME (0x356C - 0x3588)
        // ========================================
        database[0x356C] = QStringLiteral("Pantheon post");
        database[0x3570] = QStringLiteral("Ionian post");
        database[0x3574] = QStringLiteral("Corinthian post");
        database[0x3578] = QStringLiteral("Broken post");
        database[0x357C] = QStringLiteral("Pyramid");
        database[0x3580] = QStringLiteral("Sphinx");
        database[0x3584] = QStringLiteral("Mummy's casket");
        database[0x3588] = QStringLiteral("Spaceman");

        // ========================================
        // FURNITURE - SPORTS & GAMES (0x358C - 0x35B4)
        // ========================================
        database[0x358C] = QStringLiteral("Tennis table");
        database[0x3590] = QStringLiteral("Bowling pins");
        database[0x3594] = QStringLiteral("Basketball hoop");
        database[0x3598] = QStringLiteral("Ball return");
        database[0x359C] = QStringLiteral("Billiard table");
        database[0x35A0] = QStringLiteral("Mahjong table");
        database[0x35A4] = QStringLiteral("Dart board");
        database[0x35A8] = QStringLiteral("Pinball machine");
        database[0x35AC] = QStringLiteral("Arcade machine");
        database[0x35B0] = QStringLiteral("Tabletop game");
        database[0x35B4] = QStringLiteral("Stool");

        // ========================================
        // FURNITURE - WORLD LANDMARKS (0x35B8 - 0x35DC)
        // ========================================
        database[0x35B8] = QStringLiteral("Moai statue");
        database[0x35BC] = QStringLiteral("Tribal mask");
        database[0x35C0] = QStringLiteral("Arc de Triomphe");
        database[0x35C4] = QStringLiteral("Merlion");
        database[0x35C8] = QStringLiteral("Pagoda");
        database[0x35CC] = QStringLiteral("Plate armor");
        database[0x35D0] = QStringLiteral("Mermaid statue");
        database[0x35D4] = QStringLiteral("Mouth of Truth");
        database[0x35D8] = QStringLiteral("Manekin Pis");
        database[0x35DC] = QStringLiteral("Tower of Pisa");

        // ========================================
        // FURNITURE - CAMPING THEME (0x35E0 - 0x3608)
        // ========================================
        database[0x35E0] = QStringLiteral("Compass");
        database[0x35E4] = QStringLiteral("Matryoshka");
        database[0x35E8] = QStringLiteral("Backpack");
        database[0x35EC] = QStringLiteral("Mountain bike");
        database[0x35F0] = QStringLiteral("Propane stove");
        database[0x35F4] = QStringLiteral("Cooler");
        database[0x35F8] = QStringLiteral("Kayak");
        database[0x35FC] = QStringLiteral("Lantern");
        database[0x3600] = QStringLiteral("Sleeping bag");
        database[0x3604] = QStringLiteral("Campfire");
        database[0x3608] = QStringLiteral("Bonfire");

        // ========================================
        // FURNITURE - BONSAI TREES (0x360C - 0x3630)
        // ========================================
        database[0x360C] = QStringLiteral("Ponderosa bonsai");
        database[0x3610] = QStringLiteral("Pine bonsai");
        database[0x3614] = QStringLiteral("Mugho bonsai");
        database[0x3618] = QStringLiteral("Plum bonsai");
        database[0x361C] = QStringLiteral("Holly bonsai");
        database[0x3620] = QStringLiteral("Azalea bonsai");
        database[0x3624] = QStringLiteral("Maple bonsai");
        database[0x3628] = QStringLiteral("Hawthorn bonsai");
        database[0x362C] = QStringLiteral("Quince bonsai");
        database[0x3630] = QStringLiteral("Jasmine bonsai");

        // ========================================
        // FURNITURE - HOUSEPLANTS (0x3634 - 0x3668)
        // ========================================
        database[0x3634] = QStringLiteral("Pothos");
        database[0x3638] = QStringLiteral("Coconut palm");
        database[0x363C] = QStringLiteral("Pachira");
        database[0x3640] = QStringLiteral("Caladium");
        database[0x3644] = QStringLiteral("Croton");
        database[0x3648] = QStringLiteral("Dracaena");
        database[0x364C] = QStringLiteral("Lady palm");
        database[0x3650] = QStringLiteral("Cactus");
        database[0x3654] = QStringLiteral("Rubber tree");
        database[0x3658] = QStringLiteral("Snake plant");
        database[0x365C] = QStringLiteral("Fan palm");
        database[0x3660] = QStringLiteral("Corn plant");
        database[0x3664] = QStringLiteral("Bromeliaceae");
        database[0x3668] = QStringLiteral("Weeping fig");

        // ========================================
        // FURNITURE - TVS & KITCHEN (0x366C - 0x36A4)
        // ========================================
        database[0x366C] = QStringLiteral("TV with a VCR");
        database[0x3670] = QStringLiteral("Wide-screen TV");
        database[0x3674] = QStringLiteral("Retro TV");
        database[0x3678] = QStringLiteral("Kitchen sink");
        database[0x367C] = QStringLiteral("Kitchen corner");
        database[0x3680] = QStringLiteral("Deluxe range");
        database[0x3684] = QStringLiteral("Gas range");
        database[0x3688] = QStringLiteral("Range");
        database[0x368C] = QStringLiteral("Kitchen stove");
        database[0x3690] = QStringLiteral("Refrigerator");
        database[0x3694] = QStringLiteral("Retro fridge");
        database[0x3698] = QStringLiteral("Microwave");
        database[0x369C] = QStringLiteral("Toaster");
        database[0x36A0] = QStringLiteral("Sink");
        database[0x36A4] = QStringLiteral("Mixer");

        // ========================================
        // FURNITURE - BEACH THEME (0x36A8 - 0x36C8)
        // ========================================
        database[0x36A8] = QStringLiteral("Clothesline pole");
        database[0x36AC] = QStringLiteral("Doghouse");
        database[0x36B0] = QStringLiteral("Beach chair");
        database[0x36B4] = QStringLiteral("Wave breaker");
        database[0x36B8] = QStringLiteral("Surfboard");
        database[0x36BC] = QStringLiteral("Diver Dan");
        database[0x36C0] = QStringLiteral("Beach table");
        database[0x36C4] = QStringLiteral("Treasure chest");
        database[0x36C8] = QStringLiteral("Life ring");

        // ========================================
        // FURNITURE - WINTER THEME (0x36CC - 0x36D4)
        // ========================================
        database[0x36CC] = QStringLiteral("Snowboard");
        database[0x36D0] = QStringLiteral("Sleigh");
        database[0x36D4] = QStringLiteral("Snowman");

        // ========================================
        // FURNITURE - FLOWERS & BATHROOM (0x36D8 - 0x36EC)
        // ========================================
        database[0x36D8] = QStringLiteral("Sunflower");
        database[0x36DC] = QStringLiteral("Daffodil");
        database[0x36E0] = QStringLiteral("Gerbera");
        database[0x36E4] = QStringLiteral("Toilet");
        database[0x36E8] = QStringLiteral("Super toilet");
        database[0x36EC] = QStringLiteral("Men's toilet");

        // ========================================
        // FURNITURE - BOXES & STORAGE (0x36F0 - 0x3700)
        // ========================================
        database[0x36F0] = QStringLiteral("Pink box");
        database[0x36F4] = QStringLiteral("Blue box");
        database[0x36F8] = QStringLiteral("Beige box");
        database[0x36FC] = QStringLiteral("Safe");
        database[0x3700] = QStringLiteral("Piggy bank");

        // ========================================
        // FURNITURE - LAUNDRY & HEATING (0x3704 - 0x371C)
        // ========================================
        database[0x3704] = QStringLiteral("Deluxe washer");
        database[0x3708] = QStringLiteral("Automatic washer");
        database[0x370C] = QStringLiteral("Washer/dryer");
        database[0x3710] = QStringLiteral("Potbelly stove");
        database[0x3714] = QStringLiteral("Space heater");
        database[0x3718] = QStringLiteral("Fireplace");
        database[0x371C] = QStringLiteral("Radiator");

        // ========================================
        // FURNITURE - STEREOS & MUSIC (0x3720 - 0x374C)
        // ========================================
        database[0x3720] = QStringLiteral("CD player");
        database[0x3724] = QStringLiteral("Gold stereo");
        database[0x3728] = QStringLiteral("Hi-fi stereo");
        database[0x372C] = QStringLiteral("Turntable");
        database[0x3730] = QStringLiteral("Dice stereo");
        database[0x3734] = QStringLiteral("White boom box");
        database[0x3738] = QStringLiteral("Red boom box");
        database[0x373C] = QStringLiteral("Phonograph");
        database[0x3740] = QStringLiteral("Reel-to-reel");
        database[0x3744] = QStringLiteral("Retro stereo");
        database[0x3748] = QStringLiteral("High-end stereo");
        database[0x374C] = QStringLiteral("Tape deck");

        // ========================================
        // FURNITURE - CLOCKS (0x3750 - 0x376C)
        // ========================================
        database[0x3750] = QStringLiteral("Red clock");
        database[0x3754] = QStringLiteral("Kitschy clock");
        database[0x3758] = QStringLiteral("Antique clock");
        database[0x375C] = QStringLiteral("Cube clock");
        database[0x3760] = QStringLiteral("Odd clock");
        database[0x3764] = QStringLiteral("Glow clock");
        database[0x3768] = QStringLiteral("Owl clock");
        database[0x376C] = QStringLiteral("Alarm clock");

        // ========================================
        // FURNITURE - ECONO CHAIRS & MISC (0x3770 - 0x37BC)
        // ========================================
        database[0x3770] = QStringLiteral("Jade econo-chair");
        database[0x3774] = QStringLiteral("Ruby econo-chair");
        database[0x3778] = QStringLiteral("Gold econo-chair");
        database[0x377C] = QStringLiteral("Rocking chair");
        database[0x3780] = QStringLiteral("Gas pump");
        database[0x3784] = QStringLiteral("Extinguisher");
        database[0x3788] = QStringLiteral("Fire hydrant");
        database[0x378C] = QStringLiteral("Racoon figurine");
        database[0x3790] = QStringLiteral("Garbage");
        database[0x3794] = QStringLiteral("Barber's pole");
        database[0x3798] = QStringLiteral("Candy machine");
        database[0x379C] = QStringLiteral("Big festive tree");
        database[0x37A0] = QStringLiteral("Festive tree");
        database[0x37A4] = QStringLiteral("Scarecrow");
        database[0x37A8] = QStringLiteral("Birdcage");
        database[0x37AC] = QStringLiteral("Chocolates");
        database[0x37B0] = QStringLiteral("Nintendo bench");
        database[0x37B4] = QStringLiteral("Outdoor bath");
        database[0x37B8] = QStringLiteral("Screen");
        database[0x37BC] = QStringLiteral("Computer");

        // ========================================
        // FURNITURE - LIVING ROOM (0x37C0 - 0x3820)
        // ========================================
        database[0x37C0] = QStringLiteral("Pineapple bed");
        database[0x37C4] = QStringLiteral("Glass-top table");
        database[0x37C8] = QStringLiteral("Cream sofa");
        database[0x37CC] = QStringLiteral("Storage case");
        database[0x37D0] = QStringLiteral("Clothes closet");
        database[0x37D4] = QStringLiteral("Claw-foot tub");
        database[0x37D8] = QStringLiteral("Shower");
        database[0x37DC] = QStringLiteral("Box of tissues");
        database[0x37E0] = QStringLiteral("Massage chair");
        database[0x37E4] = QStringLiteral("Vacuum cleaner");
        database[0x37E8] = QStringLiteral("Bottled ship");
        database[0x37EC] = QStringLiteral("Water bird");
        database[0x37F0] = QStringLiteral("Executive toy");
        database[0x37F4] = QStringLiteral("Garbage");
        database[0x37F8] = QStringLiteral("Telescope");
        database[0x37FC] = QStringLiteral("Mochi pestle");
        database[0x3800] = QStringLiteral("Lovely phone");
        database[0x3804] = QStringLiteral("Sewing machine");
        database[0x3808] = QStringLiteral("Cardboard box");
        database[0x380C] = QStringLiteral("Wooden box");
        database[0x3810] = QStringLiteral("Miniature car");
        database[0x3814] = QStringLiteral("Fan");
        database[0x3818] = QStringLiteral("Birthday cake");
        database[0x381C] = QStringLiteral("Jack-in-the-box");
        database[0x3820] = QStringLiteral("Throne");

        // ========================================
        // FURNITURE - LAMPS (0x3824 - 0x384C)
        // ========================================
        database[0x3824] = QStringLiteral("Table lamp");
        database[0x3828] = QStringLiteral("Desk light");
        database[0x382C] = QStringLiteral("Candle");
        database[0x3830] = QStringLiteral("Mini-lamp");
        database[0x3834] = QStringLiteral("Festive flag");
        database[0x3838] = QStringLiteral("Festive candle");
        database[0x383C] = QStringLiteral("Garbage");
        database[0x3840] = QStringLiteral("Garbage");
        database[0x3844] = QStringLiteral("Garbage");
        database[0x3848] = QStringLiteral("Garbage");
        database[0x384C] = QStringLiteral("Music box");

        // ========================================
        // FURNITURE - MODELS & TROPHIES (0x3850 - 0x3880)
        // ========================================
        database[0x3850] = QStringLiteral("Portrait");
        database[0x3854] = QStringLiteral("One-story model");
        database[0x3858] = QStringLiteral("Two-story model");
        database[0x385C] = QStringLiteral("Mansion model");
        database[0x3860] = QStringLiteral("Nook's Cranny");
        database[0x3864] = QStringLiteral("Nook 'n' Go");
        database[0x3868] = QStringLiteral("Nookway");
        database[0x386C] = QStringLiteral("Nookington's");
        database[0x3870] = QStringLiteral("Museum model");
        database[0x3874] = QStringLiteral("Town hall model");
        database[0x3878] = QStringLiteral("Fish trophy");
        database[0x387C] = QStringLiteral("Bug trophy");
        database[0x3880] = QStringLiteral("Flower trophy");

        // ========================================
        // FURNITURE - BASIC BEDS (0x3884 - 0x3890)
        // ========================================
        database[0x3884] = QStringLiteral("Basic red bed");
        database[0x3888] = QStringLiteral("Basic blue bed");
        database[0x388C] = QStringLiteral("Basic yellow bed");
        database[0x3890] = QStringLiteral("Basic green bed");

        // ========================================
        // PAINTINGS - AUTHENTIC (0x3894 - 0x38E0)
        // ========================================
        database[0x3894] = QStringLiteral("Dainty painting");
        database[0x3898] = QStringLiteral("Solemn painting");
        database[0x389C] = QStringLiteral("Quaint painting");
        database[0x38A0] = QStringLiteral("Basic painting");
        database[0x38A4] = QStringLiteral("Famous painting");
        database[0x38A8] = QStringLiteral("Perfect painting");
        database[0x38AC] = QStringLiteral("Amazing painting");
        database[0x38B0] = QStringLiteral("Nice painting");
        database[0x38B4] = QStringLiteral("Moving painting");
        database[0x38B8] = QStringLiteral("Common painting");
        database[0x38BC] = QStringLiteral("Flowery painting");
        database[0x38C0] = QStringLiteral("Warm painting");
        database[0x38C4] = QStringLiteral("Rare painting");
        database[0x38C8] = QStringLiteral("Fine painting");
        database[0x38CC] = QStringLiteral("Scary painting");
        database[0x38D0] = QStringLiteral("Lovely painting");
        database[0x38D4] = QStringLiteral("Strange painting");
        database[0x38D8] = QStringLiteral("Worthy painting");
        database[0x38DC] = QStringLiteral("Calm painting");
        database[0x38E0] = QStringLiteral("Opulent painting");

        // ========================================
        // PAINTINGS - FORGERIES (0x38E4 - 0x3930)
        // ========================================
        database[0x38E4] = QStringLiteral("Dainty painting (forgery)");
        database[0x38E8] = QStringLiteral("Solemn painting (forgery)");
        database[0x38EC] = QStringLiteral("Quaint painting (forgery)");
        database[0x38F0] = QStringLiteral("Basic painting (forgery)");
        database[0x38F4] = QStringLiteral("Famous painting (forgery)");
        database[0x38F8] = QStringLiteral("Perfect painting (forgery)");
        database[0x38FC] = QStringLiteral("Awesome painting (forgery)");
        database[0x3900] = QStringLiteral("Nice painting (forgery)");
        database[0x3904] = QStringLiteral("Moving painting (forgery)");
        database[0x3908] = QStringLiteral("Common painting (forgery)");
        database[0x390C] = QStringLiteral("Flowery painting (forgery)");
        database[0x3910] = QStringLiteral("Warm painting (forgery)");
        database[0x3914] = QStringLiteral("Rare painting (forgery)");
        database[0x3918] = QStringLiteral("Fine painting (forgery)");
        database[0x391C] = QStringLiteral("Scary painting (forgery)");
        database[0x3920] = QStringLiteral("Lovely painting (forgery)");
        database[0x3924] = QStringLiteral("Strange painting (forgery)");
        database[0x3928] = QStringLiteral("Worthy painting (forgery)");
        database[0x392C] = QStringLiteral("Calm painting (forgery)");
        database[0x3930] = QStringLiteral("Opulent painting (forgery)");

        // ========================================
        // MUSEUM DISPLAYS - INSECTS (0x42A4 - 0x4380)
        // Note: Museum displays use every 4th hex value
        // ========================================
        database[0x42A4] = QStringLiteral("Common butterfly");
        database[0x42A8] = QStringLiteral("Yellow butterfly");
        database[0x42AC] = QStringLiteral("Tiger butterfly");
        database[0x42B0] = QStringLiteral("Peacock");
        database[0x42B4] = QStringLiteral("Monarch");
        database[0x42B8] = QStringLiteral("Emperor");
        database[0x42BC] = QStringLiteral("Agrias butterfly");
        database[0x42C0] = QStringLiteral("Birdwing");
        database[0x42C4] = QStringLiteral("Moth");
        database[0x42C8] = QStringLiteral("Oak silk moth");
        database[0x42CC] = QStringLiteral("Honeybee");
        database[0x42D0] = QStringLiteral("Hornet");
        database[0x42D4] = QStringLiteral("Long locust");
        database[0x42D8] = QStringLiteral("Migratory locust");
        database[0x42DC] = QStringLiteral("Mantis");
        database[0x42E0] = QStringLiteral("Orchid mantis");
        database[0x42E4] = QStringLiteral("Brown cicada");
        database[0x42E8] = QStringLiteral("Robust cicada");
        database[0x42EC] = QStringLiteral("Walker cicada");
        database[0x42F0] = QStringLiteral("Evening cicada");
        database[0x42F4] = QStringLiteral("Lanternfly");
        database[0x42F8] = QStringLiteral("Red dragonfly");
        database[0x42FC] = QStringLiteral("Darner dragonfly");
        database[0x4300] = QStringLiteral("Banded dragonfly");
        database[0x4304] = QStringLiteral("Ant");
        database[0x4308] = QStringLiteral("Pond skater");
        database[0x430C] = QStringLiteral("Snail");
        database[0x4310] = QStringLiteral("Cricket");
        database[0x4314] = QStringLiteral("Bell cricket");
        database[0x4318] = QStringLiteral("Grasshopper");
        database[0x431C] = QStringLiteral("Mole cricket");
        database[0x4320] = QStringLiteral("Walkingstick");
        database[0x4324] = QStringLiteral("Ladybug");
        database[0x4328] = QStringLiteral("Fruit beetle");
        database[0x432C] = QStringLiteral("Shining beetle");
        database[0x4330] = QStringLiteral("Dung beetle");
        database[0x4334] = QStringLiteral("Goliath beetle");
        database[0x4338] = QStringLiteral("Firefly");
        database[0x433C] = QStringLiteral("Jewel beetle");
        database[0x4340] = QStringLiteral("Longhorn beetle");
        database[0x4344] = QStringLiteral("Saw stag beetle");
        database[0x4348] = QStringLiteral("Stag beetle");
        database[0x434C] = QStringLiteral("Giant beetle");
        database[0x4350] = QStringLiteral("King stag beetle");
        database[0x4354] = QStringLiteral("Dynastid beetle");
        database[0x4358] = QStringLiteral("Atlas beetle");
        database[0x435C] = QStringLiteral("Elephant beetle");
        database[0x4360] = QStringLiteral("Hercules beetle");
        database[0x4364] = QStringLiteral("Flea");
        database[0x4368] = QStringLiteral("Pillbug");
        database[0x436C] = QStringLiteral("Mosquito");
        database[0x4370] = QStringLiteral("Fly");
        database[0x4374] = QStringLiteral("Cockroach");
        database[0x4378] = QStringLiteral("Spider");
        database[0x437C] = QStringLiteral("Tarantula");
        database[0x4380] = QStringLiteral("Scorpion");

        // ========================================
        // MUSEUM DISPLAYS - FISH (0x4384 - 0x4460)
        // Note: Museum displays use every 4th hex value
        // ========================================
        database[0x4384] = QStringLiteral("Bitterling");
        database[0x4388] = QStringLiteral("Pale chub");
        database[0x438C] = QStringLiteral("Crucian carp");
        database[0x4390] = QStringLiteral("Dace");
        database[0x4394] = QStringLiteral("Barbel steed");
        database[0x4398] = QStringLiteral("Carp");
        database[0x439C] = QStringLiteral("Koi");
        database[0x43A0] = QStringLiteral("Goldfish");
        database[0x43A4] = QStringLiteral("Popeyed goldfish");
        database[0x43A8] = QStringLiteral("Killifish");
        database[0x43AC] = QStringLiteral("Crawfish");
        database[0x43B0] = QStringLiteral("Frog");
        database[0x43B4] = QStringLiteral("Freshwater goby");
        database[0x43B8] = QStringLiteral("Loach");
        database[0x43BC] = QStringLiteral("Catfish");
        database[0x43C0] = QStringLiteral("Eel");
        database[0x43C4] = QStringLiteral("Giant snakehead");
        database[0x43C8] = QStringLiteral("Bluegill");
        database[0x43CC] = QStringLiteral("Yellow perch");
        database[0x43D0] = QStringLiteral("Black bass");
        database[0x43D4] = QStringLiteral("Pond smelt");
        database[0x43D8] = QStringLiteral("Sweetfish");
        database[0x43DC] = QStringLiteral("Cherry salmon");
        database[0x43E0] = QStringLiteral("Char");
        database[0x43E4] = QStringLiteral("Rainbow trout");
        database[0x43E8] = QStringLiteral("Stringfish");
        database[0x43EC] = QStringLiteral("Salmon");
        database[0x43F0] = QStringLiteral("King salmon");
        database[0x43F4] = QStringLiteral("Guppy");
        database[0x43F8] = QStringLiteral("Angelfish");
        database[0x43FC] = QStringLiteral("Piranha");
        database[0x4400] = QStringLiteral("Arowana");
        database[0x4404] = QStringLiteral("Dorado");
        database[0x4408] = QStringLiteral("Gar");
        database[0x440C] = QStringLiteral("Arapaima");
        database[0x4410] = QStringLiteral("Sea butterfly");
        database[0x4414] = QStringLiteral("Jellyfish");
        database[0x4418] = QStringLiteral("Seahorse");
        database[0x441C] = QStringLiteral("Clownfish");
        database[0x4420] = QStringLiteral("Zebra turkeyfish");
        database[0x4424] = QStringLiteral("Balloon fish");
        database[0x4428] = QStringLiteral("Horse mackerel");
        database[0x442C] = QStringLiteral("Barred knifejaw");
        database[0x4430] = QStringLiteral("Sea bass");
        database[0x4434] = QStringLiteral("Red snapper");
        database[0x4438] = QStringLiteral("Dab");
        database[0x443C] = QStringLiteral("Olive flounder");
        database[0x4440] = QStringLiteral("Squid");
        database[0x4444] = QStringLiteral("Octopus");
        database[0x4448] = QStringLiteral("Football fish");
        database[0x444C] = QStringLiteral("Tuna");
        database[0x4450] = QStringLiteral("Blue marlin");
        database[0x4454] = QStringLiteral("Ocean sunfish");
        database[0x4458] = QStringLiteral("Hammerhead shark");
        database[0x445C] = QStringLiteral("Shark");
        database[0x4460] = QStringLiteral("Coelacanth");

        // ========================================
        // MUSEUM DISPLAYS - FLOWERS (0x4464 - 0x44E4)
        // Note: Museum displays use every 4th hex value
        // ========================================
        database[0x4464] = QStringLiteral("Red tulips");
        database[0x4468] = QStringLiteral("White tulips");
        database[0x446C] = QStringLiteral("Yellow tulips");
        database[0x4470] = QStringLiteral("Pink tulips");
        database[0x4474] = QStringLiteral("Purple tulips");
        database[0x4478] = QStringLiteral("Black tulips");
        database[0x447C] = QStringLiteral("White pansies");
        database[0x4480] = QStringLiteral("Yellow pansies");
        database[0x4484] = QStringLiteral("Red pansies");
        database[0x4488] = QStringLiteral("Purple pansies");
        database[0x448C] = QStringLiteral("Orange pansies");
        database[0x4490] = QStringLiteral("Blue pansies");
        database[0x4494] = QStringLiteral("White cosmos");
        database[0x4498] = QStringLiteral("Red cosmos");
        database[0x449C] = QStringLiteral("Yellow cosmos");
        database[0x44A0] = QStringLiteral("Pink cosmos");
        database[0x44A4] = QStringLiteral("Orange cosmos");
        database[0x44A8] = QStringLiteral("Black cosmos");
        database[0x44AC] = QStringLiteral("Red roses");
        database[0x44B0] = QStringLiteral("White roses");
        database[0x44B4] = QStringLiteral("Yellow roses");
        database[0x44B8] = QStringLiteral("Pink roses");
        database[0x44BC] = QStringLiteral("Orange roses");
        database[0x44C0] = QStringLiteral("Purple roses");
        database[0x44C4] = QStringLiteral("Black roses");
        database[0x44C8] = QStringLiteral("Blue roses");
        database[0x44CC] = QStringLiteral("Jacob's Ladder");
        database[0x44D0] = QStringLiteral("Garbage");
        database[0x44D4] = QStringLiteral("Gold roses");
        database[0x44D8] = QStringLiteral("Dandelions");
        database[0x44DC] = QStringLiteral("Dandelion puffs");
        database[0x44E0] = QStringLiteral("Garbage");
        database[0x44E4] = QStringLiteral("Lucky clovers");

        // ========================================
        // MUSEUM DISPLAYS - MISC (0x44E8 - 0x4508)
        // Note: Museum displays use every 4th hex value
        // ========================================
        database[0x44E8] = QStringLiteral("Spoiled turnips");
        database[0x44EC] = QStringLiteral("Conch");
        database[0x44F0] = QStringLiteral("White scallop");
        database[0x44F4] = QStringLiteral("Coral");
        database[0x44F8] = QStringLiteral("Venus comb");
        database[0x44FC] = QStringLiteral("Scallop");
        database[0x4500] = QStringLiteral("Shell");
        database[0x4504] = QStringLiteral("Porceletta");
        database[0x4508] = QStringLiteral("Sand dollar");

        // ========================================
        // MUSEUM DISPLAYS - FOSSILS (0x450C - 0x45D8)
        // Note: Museum displays use every 4th hex value
        // ========================================
        database[0x450C] = QStringLiteral("Amber");
        database[0x4510] = QStringLiteral("Ammonite");
        database[0x4514] = QStringLiteral("Dino droppings");
        database[0x4518] = QStringLiteral("Dinosaur egg");
        database[0x451C] = QStringLiteral("Fern fossil");
        database[0x4520] = QStringLiteral("Dinosaur track");
        database[0x4524] = QStringLiteral("Archaeopteryx");
        database[0x4528] = QStringLiteral("Peking Man");
        database[0x452C] = QStringLiteral("Shark tooth");
        database[0x4530] = QStringLiteral("Trilobite");
        database[0x4534] = QStringLiteral("T-rex skull");
        database[0x4538] = QStringLiteral("T-rex torso");
        database[0x453C] = QStringLiteral("T-rex tail");
        database[0x4540] = QStringLiteral("Tricera skull");
        database[0x4544] = QStringLiteral("Tricera torso");
        database[0x4548] = QStringLiteral("Tricera tail");
        database[0x454C] = QStringLiteral("Mammoth skull");
        database[0x4550] = QStringLiteral("Mammoth torso");
        database[0x4554] = QStringLiteral("Ankylo skull");
        database[0x4558] = QStringLiteral("Ankylo torso");
        database[0x455C] = QStringLiteral("Ankylo tail");
        database[0x4560] = QStringLiteral("Apato skull");
        database[0x4564] = QStringLiteral("Apato torso");
        database[0x4568] = QStringLiteral("Apato tail");
        database[0x456C] = QStringLiteral("Dimetrodon skull");
        database[0x4570] = QStringLiteral("Dimetrodon torso");
        database[0x4574] = QStringLiteral("Dimetrodon tail");
        database[0x4578] = QStringLiteral("Iguanodon skull");
        database[0x457C] = QStringLiteral("Iguanodon torso");
        database[0x4580] = QStringLiteral("Iguanodon tail");
        database[0x4584] = QStringLiteral("Sabertooth skull");
        database[0x4588] = QStringLiteral("Sabertooth torso");
        database[0x458C] = QStringLiteral("Pachy skull");
        database[0x4590] = QStringLiteral("Pachy torso");
        database[0x4594] = QStringLiteral("Pachy tail");
        database[0x4598] = QStringLiteral("Parasaur skull");
        database[0x459C] = QStringLiteral("Parasaur torso");
        database[0x45A0] = QStringLiteral("Parasaur tail");
        database[0x45A4] = QStringLiteral("Seismo skull");
        database[0x45A8] = QStringLiteral("Seismo chest");
        database[0x45AC] = QStringLiteral("Seismo hip");
        database[0x45B0] = QStringLiteral("Seismo tail");
        database[0x45B4] = QStringLiteral("Plesio skull");
        database[0x45B8] = QStringLiteral("Plesio neck");
        database[0x45BC] = QStringLiteral("Plesio torso");
        database[0x45C0] = QStringLiteral("Stego skull");
        database[0x45C4] = QStringLiteral("Stego torso");
        database[0x45C8] = QStringLiteral("Stego tail");
        database[0x45CC] = QStringLiteral("Ptera skull");
        database[0x45D0] = QStringLiteral("Ptera body");
        database[0x45D4] = QStringLiteral("Ptera left wing");
        database[0x45D8] = QStringLiteral("Ptera right wing");

        // ========================================
        // MUSEUM DISPLAYS - GYROIDS (0x45DC - 0x472C)
        // Note: Museum displays use every 4th hex value
        // ========================================
        database[0x45DC] = QStringLiteral("Mega clankoid");
        database[0x45E0] = QStringLiteral("Clankoid");
        database[0x45E4] = QStringLiteral("Mini clankoid");
        database[0x45E8] = QStringLiteral("Tall clankoid");
        database[0x45EC] = QStringLiteral("Squat dingloid");
        database[0x45F0] = QStringLiteral("Mega dingloid");
        database[0x45F4] = QStringLiteral("Dingloid");
        database[0x45F8] = QStringLiteral("Mini dingloid");
        database[0x45FC] = QStringLiteral("Wee dingloid");
        database[0x4600] = QStringLiteral("Tall dingloid");
        database[0x4604] = QStringLiteral("Mega percoloid");
        database[0x4608] = QStringLiteral("Tall percoloid");
        database[0x460C] = QStringLiteral("Mega oombloid");
        database[0x4610] = QStringLiteral("Oombloid");
        database[0x4614] = QStringLiteral("Mini oombloid");
        database[0x4618] = QStringLiteral("Tall oombloid");
        database[0x461C] = QStringLiteral("Rhythmoid");
        database[0x4620] = QStringLiteral("Mini rhythmoid");
        database[0x4624] = QStringLiteral("Mega puffoid");
        database[0x4628] = QStringLiteral("Mini puffoid");
        database[0x462C] = QStringLiteral("Tall puffoid");
        database[0x4630] = QStringLiteral("Mega lullaboid");
        database[0x4634] = QStringLiteral("Lullaboid");
        database[0x4638] = QStringLiteral("Mini lullaboid");
        database[0x463C] = QStringLiteral("Tall lullaboid");
        database[0x4640] = QStringLiteral("Mega sproid");
        database[0x4644] = QStringLiteral("Sproid");
        database[0x4648] = QStringLiteral("Mini sproid");
        database[0x464C] = QStringLiteral("Tall sproid");
        database[0x4650] = QStringLiteral("Mega bowtoid");
        database[0x4654] = QStringLiteral("Bowtoid");
        database[0x4658] = QStringLiteral("Mini bowtoid");
        database[0x465C] = QStringLiteral("Tall bowtoid");
        database[0x4660] = QStringLiteral("Mega tootoid");
        database[0x4664] = QStringLiteral("Tootoid");
        database[0x4668] = QStringLiteral("Mini tootoid");
        database[0x466C] = QStringLiteral("Mega plinkoid");
        database[0x4670] = QStringLiteral("Plinkoid");
        database[0x4674] = QStringLiteral("Mini plinkoid");
        database[0x4678] = QStringLiteral("Mega dekkoid");
        database[0x467C] = QStringLiteral("Dekkoid");
        database[0x4680] = QStringLiteral("Mini dekkoid");
        database[0x4684] = QStringLiteral("Mega buzzoid");
        database[0x4688] = QStringLiteral("Buzzoid");
        database[0x468C] = QStringLiteral("Mini buzzoid");
        database[0x4690] = QStringLiteral("Tall buzzoid");
        database[0x4694] = QStringLiteral("Mega drilloid");
        database[0x4698] = QStringLiteral("Drilloid");
        database[0x469C] = QStringLiteral("Mini drilloid");
        database[0x46A0] = QStringLiteral("Mega echoid");
        database[0x46A4] = QStringLiteral("Mini echoid");
        database[0x46A8] = QStringLiteral("Tall echoid");
        database[0x46AC] = QStringLiteral("Mega lamentoid");
        database[0x46B0] = QStringLiteral("Lamentoid");
        database[0x46B4] = QStringLiteral("Mini lamentoid");
        database[0x46B8] = QStringLiteral("Tall lamentoid");
        database[0x46BC] = QStringLiteral("Mega strumboid");
        database[0x46C0] = QStringLiteral("Strumboid");
        database[0x46C4] = QStringLiteral("Mini strumboid");
        database[0x46C8] = QStringLiteral("Tall strumboid");
        database[0x46CC] = QStringLiteral("Mega croakoid");
        database[0x46D0] = QStringLiteral("Croakoid");
        database[0x46D4] = QStringLiteral("Mini croakoid");
        database[0x46D8] = QStringLiteral("Tall croakoid");
        database[0x46DC] = QStringLiteral("Mega fizzoid");
        database[0x46E0] = QStringLiteral("Mini fizzoid");
        database[0x46E4] = QStringLiteral("Mega squelchoid");
        database[0x46E8] = QStringLiteral("Squelchoid");
        database[0x46EC] = QStringLiteral("Mini squelchoid");
        database[0x46F0] = QStringLiteral("Mega howloid");
        database[0x46F4] = QStringLiteral("Howloid");
        database[0x46F8] = QStringLiteral("Mini howloid");
        database[0x46FC] = QStringLiteral("Mega nebuloid");
        database[0x4700] = QStringLiteral("Squat nebuloid");
        database[0x4704] = QStringLiteral("Nebuloid");
        database[0x4708] = QStringLiteral("Mini nebuloid");
        database[0x470C] = QStringLiteral("Tall nebuloid");
        database[0x4710] = QStringLiteral("Slim nebuloid");
        database[0x4714] = QStringLiteral("Mega gongoid");
        database[0x4718] = QStringLiteral("Gongoid");
        database[0x471C] = QStringLiteral("Mini gongoid");
        database[0x4720] = QStringLiteral("Tall gongoid");
        database[0x4724] = QStringLiteral("Mega poltergoid");
        database[0x4728] = QStringLiteral("Poltergoid");
        database[0x472C] = QStringLiteral("Mini poltergoid");

        // ========================================
        // Villager & NPC Pictures (0x47D8 - 0x4A44)
        // ========================================
        database[0x47D8] = QStringLiteral("Cyrano's pic");
        database[0x47DC] = QStringLiteral("Antonio's pic");
        database[0x47E0] = QStringLiteral("Pango's pic");
        database[0x47E4] = QStringLiteral("Anabelle's pic");
        database[0x47E8] = QStringLiteral("Teddy's pic");
        database[0x47EC] = QStringLiteral("Pinky's pic");
        database[0x47F0] = QStringLiteral("Curt's pic");
        database[0x47F4] = QStringLiteral("Chow's pic");
        database[0x47F8] = QStringLiteral("Jay's pic");
        database[0x47FC] = QStringLiteral("Robin's pic");
        database[0x4800] = QStringLiteral("Anchovy's pic");
        database[0x4804] = QStringLiteral("Twiggy's pic");
        database[0x4808] = QStringLiteral("Jitters's pic");
        database[0x480C] = QStringLiteral("Angus's pic");
        database[0x4810] = QStringLiteral("Rodeo's pic");
        database[0x4814] = QStringLiteral("Bob's pic");
        database[0x4818] = QStringLiteral("Mitzi's pic");
        database[0x481C] = QStringLiteral("Rosie's pic");
        database[0x4820] = QStringLiteral("Olivia's pic");
        database[0x4824] = QStringLiteral("Kiki's pic");
        database[0x4828] = QStringLiteral("Tangy's pic");
        database[0x482C] = QStringLiteral("Punchy's pic");
        database[0x4830] = QStringLiteral("Purrl's pic");
        database[0x4834] = QStringLiteral("Moe's pic");
        database[0x4838] = QStringLiteral("Kabuki's pic");
        database[0x483C] = QStringLiteral("Kid Cat's pic");
        database[0x4840] = QStringLiteral("Monique's pic");
        database[0x4844] = QStringLiteral("Tabby's pic");
        database[0x4848] = QStringLiteral("Bluebear's pic");
        database[0x484C] = QStringLiteral("Maple's pic");
        database[0x4850] = QStringLiteral("Poncho's pic");
        database[0x4854] = QStringLiteral("Pudge's pic");
        database[0x4858] = QStringLiteral("Kody's pic");
        database[0x485C] = QStringLiteral("Stitches's pic");
        database[0x4860] = QStringLiteral("Goose's pic");
        database[0x4864] = QStringLiteral("Benedict's pic");
        database[0x4868] = QStringLiteral("Egbert's pic");
        database[0x486C] = QStringLiteral("Patty's pic");
        database[0x4870] = QStringLiteral("Tipper's pic");
        database[0x4874] = QStringLiteral("Alfonso's pic");
        database[0x4878] = QStringLiteral("Alli's pic");
        database[0x487C] = QStringLiteral("Goldie's pic");
        database[0x4880] = QStringLiteral("Butch's pic");
        database[0x4884] = QStringLiteral("Lucky's pic");
        database[0x4888] = QStringLiteral("Biskit's pic");
        database[0x488C] = QStringLiteral("Bones's pic");
        database[0x4890] = QStringLiteral("Portia's pic");
        database[0x4894] = QStringLiteral("Walker's pic");
        database[0x4898] = QStringLiteral("Daisy's pic");
        database[0x489C] = QStringLiteral("Bill's pic");
        database[0x48A0] = QStringLiteral("Joey's pic");
        database[0x48A4] = QStringLiteral("Pate's pic");
        database[0x48A8] = QStringLiteral("Maelle's pic");
        database[0x48AC] = QStringLiteral("Deena's pic");
        database[0x48B0] = QStringLiteral("Pompom's pic");
        database[0x48B4] = QStringLiteral("Mallary's pic");
        database[0x48B8] = QStringLiteral("Freckles's pic");
        database[0x48BC] = QStringLiteral("Derwin's pic");
        database[0x48C0] = QStringLiteral("Drake's pic");
        database[0x48C4] = QStringLiteral("Opal's pic");
        database[0x48C8] = QStringLiteral("Dizzy's pic");
        database[0x48CC] = QStringLiteral("Big Top's pic");
        database[0x48D0] = QStringLiteral("Eloise's pic");
        database[0x48D4] = QStringLiteral("Margie's pic");
        database[0x48D8] = QStringLiteral("Lily's pic");
        database[0x48DC] = QStringLiteral("Ribbot's pic");
        database[0x48E0] = QStringLiteral("Frobert's pic");
        database[0x48E4] = QStringLiteral("Camofrog's pic");
        database[0x48E8] = QStringLiteral("Drift's pic");
        database[0x48EC] = QStringLiteral("Wart Jr.'s pic");
        database[0x48F0] = QStringLiteral("Puddles's pic");
        database[0x48F4] = QStringLiteral("Jeremiah's pic");
        database[0x48F8] = QStringLiteral("Chevre's pic");
        database[0x48FC] = QStringLiteral("Nan's pic");
        database[0x4900] = QStringLiteral("Cesar's pic");
        database[0x4904] = QStringLiteral("Peewee's pic");
        database[0x4908] = QStringLiteral("Boone's pic");
        database[0x490C] = QStringLiteral("Rocco's pic");
        database[0x4910] = QStringLiteral("Buck's pic");
        database[0x4914] = QStringLiteral("Victoria's pic");
        database[0x4918] = QStringLiteral("Savannah's pic");
        database[0x491C] = QStringLiteral("Elmer's pic");
        database[0x4920] = QStringLiteral("Roscoe's pic");
        database[0x4924] = QStringLiteral("Yuka's pic");
        database[0x4928] = QStringLiteral("Alice's pic");
        database[0x492C] = QStringLiteral("Melba's pic");
        database[0x4930] = QStringLiteral("Kitt's pic");
        database[0x4934] = QStringLiteral("Mathilda's pic");
        database[0x4938] = QStringLiteral("Bud's pic");
        database[0x493C] = QStringLiteral("Elvis's pic");
        database[0x4940] = QStringLiteral("Dora's pic");
        database[0x4944] = QStringLiteral("Limberg's pic");
        database[0x4948] = QStringLiteral("Bella's pic");
        database[0x494C] = QStringLiteral("Bree's pic");
        database[0x4950] = QStringLiteral("Samson's pic");
        database[0x4954] = QStringLiteral("Rod's pic");
        database[0x4958] = QStringLiteral("Octavian's pic");
        database[0x495C] = QStringLiteral("Marina's pic");
        database[0x4960] = QStringLiteral("Queenie's pic");
        database[0x4964] = QStringLiteral("Gladys's pic");
        database[0x4968] = QStringLiteral("Apollo's pic");
        database[0x496C] = QStringLiteral("Amelia's pic");
        database[0x4970] = QStringLiteral("Pierce's pic");
        database[0x4974] = QStringLiteral("Aurora's pic");
        database[0x4978] = QStringLiteral("Roald's pic");
        database[0x497C] = QStringLiteral("Cube's pic");
        database[0x4980] = QStringLiteral("Hopper's pic");
        database[0x4984] = QStringLiteral("Friga's pic");
        database[0x4988] = QStringLiteral("Gwen's pic");
        database[0x498C] = QStringLiteral("Curly's pic");
        database[0x4990] = QStringLiteral("Truffles's pic");
        database[0x4994] = QStringLiteral("Rasher's pic");
        database[0x4998] = QStringLiteral("Hugh's pic");
        database[0x499C] = QStringLiteral("Lucy's pic");
        database[0x49A0] = QStringLiteral("Bunnie's pic");
        database[0x49A4] = QStringLiteral("Dotty's pic");
        database[0x49A8] = QStringLiteral("Coco's pic");
        database[0x49AC] = QStringLiteral("Snake's pic");
        database[0x49B0] = QStringLiteral("Gaston's pic");
        database[0x49B4] = QStringLiteral("Gabi's pic");
        database[0x49B8] = QStringLiteral("Pippy's pic");
        database[0x49BC] = QStringLiteral("Tiffany's pic");
        database[0x49C0] = QStringLiteral("Genji's pic");
        database[0x49C4] = QStringLiteral("Ruby's pic");
        database[0x49C8] = QStringLiteral("Tank's pic");
        database[0x49CC] = QStringLiteral("Rhonda's pic");
        database[0x49D0] = QStringLiteral("Vesta's pic");
        database[0x49D4] = QStringLiteral("Baabara's pic");
        database[0x49D8] = QStringLiteral("Peanut's pic");
        database[0x49DC] = QStringLiteral("Blaire's pic");
        database[0x49E0] = QStringLiteral("Filbert's pic");
        database[0x49E4] = QStringLiteral("Pecan's pic");
        database[0x49E8] = QStringLiteral("Nibbles's pic");
        database[0x49EC] = QStringLiteral("Agent S's pic");
        database[0x49F0] = QStringLiteral("Caroline's pic");
        database[0x49F4] = QStringLiteral("Sally's pic");
        database[0x49F8] = QStringLiteral("Static's pic");
        database[0x49FC] = QStringLiteral("Mint's pic");
        database[0x4A00] = QStringLiteral("Rolf's pic");
        database[0x4A04] = QStringLiteral("Rowan's pic");
        database[0x4A08] = QStringLiteral("Chief's pic");
        database[0x4A0C] = QStringLiteral("Lobo's pic");
        database[0x4A10] = QStringLiteral("Wolfgang's pic");
        database[0x4A14] = QStringLiteral("Whitney's pic");
        database[0x4A18] = QStringLiteral("Champ's pic");
        database[0x4A1C] = QStringLiteral("Nana's pic");
        database[0x4A20] = QStringLiteral("Simon's pic");
        database[0x4A24] = QStringLiteral("Tammi's pic");
        database[0x4A28] = QStringLiteral("Monty's pic");
        database[0x4A2C] = QStringLiteral("Elise's pic");
        // NPC Pictures
        database[0x4A30] = QStringLiteral("Tom Nook's pic");
        database[0x4A34] = QStringLiteral("K.K.'s pic");
        database[0x4A38] = QStringLiteral("Pascal's pic");
        database[0x4A3C] = QStringLiteral("K and K's pic");
        database[0x4A40] = QStringLiteral("Pelly's pic");
        database[0x4A44] = QStringLiteral("Phyllis's pic");

        // ========================================
        // SPECIAL FURNITURE (0x4A48 - 0x4A64)
        // Note: Uses every 4th hex value
        // ========================================
        database[0x4A48] = QStringLiteral("2 Cardboard Boxes");
        database[0x4A4C] = QStringLiteral("4 Cardboard Boxes");
        database[0x4A50] = QStringLiteral("5 Cardboard Boxes");
        database[0x4A54] = QStringLiteral("Display Platform");
        database[0x4A58] = QStringLiteral("Invisible Chair #1");
        database[0x4A5C] = QStringLiteral("Invisible Chair #2");
        database[0x4A60] = QStringLiteral("Big Rug #1");
        database[0x4A64] = QStringLiteral("Big Rug #2");

        // ========================================
        // NINTENDO ITEMS (0x4B68 - 0x4BA0)
        // Note: Special DLC/event items, uses every 4th hex value
        // ========================================
        database[0x4B68] = QStringLiteral("Brick Block");
        database[0x4B6C] = QStringLiteral("Coin");
        database[0x4B70] = QStringLiteral("Flagpole");
        database[0x4B74] = QStringLiteral("Fire Flower");
        database[0x4B78] = QStringLiteral("Super Mushroom");
        database[0x4B7C] = QStringLiteral("Green Pipe");
        database[0x4B80] = QStringLiteral("? Block");
        database[0x4B84] = QStringLiteral("Koopa Shell");
        database[0x4B88] = QStringLiteral("Starman");
        database[0x4B8C] = QStringLiteral("Cannon");
        database[0x4B90] = QStringLiteral("1-Up Mushroom");
        database[0x4B94] = QStringLiteral("Fire Bar");
        database[0x4B98] = QStringLiteral("Pikmin");
        database[0x4B9C] = QStringLiteral("Blue Falcon");
        database[0x4BA0] = QStringLiteral("Cardboard Box");

        initialized = true;
    }

    return database;
}

} // namespace Detail

QString getItemName(uint16_t hexCode) {
    const auto& db = Detail::getDatabase();
    auto it = db.find(hexCode);
    if (it != db.end()) {
        return it.value();
    }
    return QString();
}

QList<QPair<uint16_t, QString>> getAllItems() {
    const auto& db = Detail::getDatabase();
    QList<QPair<uint16_t, QString>> result;
    result.reserve(db.size());

    for (auto it = db.begin(); it != db.end(); ++it) {
        result.append(qMakePair(it.key(), it.value()));
    }

    return result;
}

Category getItemCategory(uint16_t hexCode) {
    using namespace Categories;

    // Stationery uses every 4th value from 0x1000 to 0x10FC
    if (hexCode >= STATIONERY_START && hexCode <= STATIONERY_END) {
        // Check if it's a valid stationery code (every 4th value)
        if ((hexCode - STATIONERY_START) % 4 == 0) {
            return Category::Stationery;
        }
    }

    if (hexCode >= WALLPAPER_START && hexCode <= WALLPAPER_END) {
        return Category::Wallpaper;
    }

    if (hexCode >= CARPET_START && hexCode <= CARPET_END) {
        return Category::Carpet;
    }

    if (hexCode >= CLOTHING_START && hexCode <= CLOTHING_END) {
        return Category::Clothing;
    }

    if (hexCode >= INSECTS_START && hexCode <= INSECTS_END) {
        return Category::Insects;
    }

    if (hexCode >= FISH_START && hexCode <= FISH_END) {
        return Category::Fish;
    }

    if (hexCode >= TRASH_START && hexCode <= TRASH_END) {
        return Category::Trash;
    }

    if (hexCode >= SONGS_START && hexCode <= SONGS_END) {
        return Category::Songs;
    }

    if (hexCode >= TOOLS_START && hexCode <= TOOLS_END) {
        return Category::Tools;
    }

    if (hexCode >= UMBRELLAS_START && hexCode <= UMBRELLAS_END) {
        return Category::Umbrellas;
    }

    if (hexCode >= HATS_START && hexCode <= HATS_END) {
        return Category::Hats;
    }

    if (hexCode >= HAIR_ACCESSORIES_START && hexCode <= HAIR_ACCESSORIES_END) {
        return Category::HairAccessories;
    }

    // Flowers range overlaps with Glasses and Facial, so check those first
    if (hexCode >= GLASSES_START && hexCode <= GLASSES_END) {
        return Category::Glasses;
    }

    if (hexCode >= FACIAL_START && hexCode <= FACIAL_END) {
        return Category::FacialAccessories;
    }

    if (hexCode >= FLOWERS_START && hexCode <= FLOWERS_END) {
        return Category::Flowers;
    }

    if (hexCode >= BELLS_START && hexCode <= BELLS_END) {
        return Category::Bells;
    }

    if (hexCode >= FURNITURE_START && hexCode <= FURNITURE_END) {
        return Category::Furniture;
    }

    if (hexCode >= PAINTINGS_START && hexCode <= PAINTINGS_END) {
        return Category::Paintings;
    }

    if (hexCode >= MUSEUM_START && hexCode <= MUSEUM_END) {
        return Category::MuseumDisplays;
    }

    if (hexCode >= PICTURES_START && hexCode <= PICTURES_END) {
        return Category::VillagerPictures;
    }

    return Category::Unknown;
}

QList<QPair<uint16_t, QString>> getItemsByCategory(Category category) {
    const auto& db = Detail::getDatabase();
    QList<QPair<uint16_t, QString>> result;

    for (auto it = db.begin(); it != db.end(); ++it) {
        if (getItemCategory(it.key()) == category) {
            result.append(qMakePair(it.key(), it.value()));
        }
    }

    return result;
}

QString getCategoryName(Category category) {
    switch (category) {
        case Category::Stationery:        return QStringLiteral("Stationery");
        case Category::Wallpaper:         return QStringLiteral("Wallpaper");
        case Category::Carpet:            return QStringLiteral("Carpet/Flooring");
        case Category::Clothing:          return QStringLiteral("Clothing");
        case Category::Insects:           return QStringLiteral("Insects");
        case Category::Fish:              return QStringLiteral("Fish");
        case Category::Trash:             return QStringLiteral("Trash");
        case Category::Songs:             return QStringLiteral("K.K. Songs");
        case Category::Tools:             return QStringLiteral("Tools");
        case Category::Umbrellas:         return QStringLiteral("Umbrellas");
        case Category::Hats:              return QStringLiteral("Hats");
        case Category::HairAccessories:   return QStringLiteral("Hair Accessories");
        case Category::Flowers:           return QStringLiteral("Flowers");
        case Category::Glasses:           return QStringLiteral("Glasses");
        case Category::FacialAccessories: return QStringLiteral("Facial Accessories");
        case Category::Bells:             return QStringLiteral("Bags of Bells");
        case Category::Furniture:         return QStringLiteral("Furniture");
        case Category::Paintings:         return QStringLiteral("Paintings");
        case Category::MuseumDisplays:    return QStringLiteral("Museum Displays");
        case Category::VillagerPictures:  return QStringLiteral("Villager Pictures");
        case Category::Unknown:
        default:                          return QStringLiteral("Unknown");
    }
}

} // namespace ItemDatabase
