#include "Adafruit_AVRProg.h"

const image_t PROGMEM image_817 = {
    // Sketch name, only used for serial printing
    {"blink_attiny817.hex"},
    // Chip name, only used for serial printing
    {"attiny817"},
    // Signature bytes for 817
    0x9320,
    // Programming fuses, written before writing to flash. Fuses set to
    // zero are untouched.
    {0x00, 0x00, 0x00, 0x00}, // {lock, low, high, extended}
    // Normal fuses, written after writing to flash (but before
    // verifying). Fuses set to zero are untouched.
    {0x0, 0x0, 0x2, 0xFF, 0x0, 0xC5, 0x04, 0x00, 0x00, 0xFF}, // {lock, low, high, extended}
    // Fuse verify mask. Any bits set to zero in these values are
    // ignored while verifying the fuses after writing them. All (and
    // only) bits that are unused for this atmega chip should be zero
    // here.
    {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF}, // {lock, low, high, extended}
    // size of chip flash in bytes
    8192,
    // size in bytes of flash page
    64,
    // The actual image to flash. This can be copy-pasted as-is from a
    // .hex file. If you do, replace all lines below starting with a
    // colon, but make sure to keep the start and end markers {R"( and
    // )"} in place.
    {R"(
:100000001AC03EC03DC03CC03BC03AC039C038C039
:1000100037C036C035C034C033C030C994C930C0D1
:100020002FC02EC02DC0E5C992CA2AC0F9C8D3C8B6
:1000300027C026C0970B11241FBECFEFCDBFDFE333
:10004000DEBF1EE3A0E0BEE3EBE2FBE102C00590F1
:100050000D92A230B107D9F72FE3A2E0BEE301C0B1
:100060001D92AC3AB207E1F710E0CBE1D0E003C05B
:100070002197FE010FDDCA31D107D1F7D1DB10CDB9
:10008000BFCFFC01508D418D252F30E0842F90E0B3
:10009000821B930B541710F00F9608950197089543
:1000A000FC0196898789981761F0A789AE0FBF2F49
:1000B000B11D5B968C9197899F5F9F70978B90E0A5
:1000C00008958FEF9FEF0895FC019689878998170F
:1000D00031F08789E80FF11D838D90E008958FEF4F
:1000E0009FEF0895FC0196892789892F90E040961B
:1000F000821B91098F7099270895CF93DF93EC01AC
:10010000E881F9810680F781E02D0995EC85FD8570
:1001100086818F738683EC85FD8585818F758583C8
:100120008F898E8B1D8ADF91CF910895CF93DF93B6
:10013000FC01908D818DA485B585981309C0159615
:100140008C9115978F7D15968C93DF91CF910895A3
:10015000C18DCE0FDF2FD11D9BA5818D8F5F8F703D
:10016000818F80E414968C93A485B58512969C9318
:10017000908D818D9813E9CF0484F585E02D8581DC
:100180008F7D8583E2CFDC011C96ED91FC910FB64B
:1001900007FE07C0258125FF04C0209111012223FD
:1001A00019F0248125FDC2CF08950F931F93CF939B
:1001B000DF93EC0181E08D8B988D898D981311C0B0
:1001C000EC85FD85848185FF0CC080E48483EC850B
:1001D000FD85628381E090E0DF91CF911F910F91C7
:1001E0000895062F188D1F5F1F70898D811303C01E
:1001F000CE01C9DFFACFE88DEC0FFD2FF11D03A76B
:10020000188FEC85FD85858180628583E3CFCF9350
:10021000DF93EC018D89882359F0EC85FD8585817C
:1002200085FD03C0848186FD03C0CE01ACDFF5CF20
:10023000DF91CF910895AF92BF92CF92DF92EF926C
:10024000FF920F931F93CF93DF936C017B018B0180
:10025000040F151FEB015E01AE18BF08C017D107D0
:1002600059F06991D601ED91FC910190F081E02D5A
:10027000C6010995892B79F7C501DF91CF911F91AF
:100280000F91FF90EF90DF90CF90BF90AF900895C7
:1002900081E00895DC01ED91FC910088F189E02D69
:1002A00023E030E00994E0E4FAE0108215828FEF59
:1002B00086838093013EE091423FF091433F8091DD
:1002C000413F319780830895E0E1F8E091859066A1
:1002D000918792E09093293F8093223F1092213F93
:1002E000089581E08093213F1092223F90911B0856
:1002F00091FF10C01092353FE0911F3FF091203FD9
:1003000009958093343F85E08093293F83E0809373
:100310001A0808951092363F8093333F86E0F4CF59
:10032000E091483FF091493F309741F010921E3FD5
:1003300010921D3F099580911D3F089580E008951A
:10034000E091463FF091473F309759F030911C3F84
:1003500020911B3F321728F010921C3F80931B3FC7
:10036000099408950895E0911C3F80911B3FE81780
:1003700030F4F0E0E550F14C808190E008958FEF8B
:100380009FEF089590911C3F80911B3F2FEF3FEF0F
:10039000981748F4E92FF0E0E550F14C208130E067
:1003A0009F5F90931C3FC901089580911B3F9091DE
:1003B0001C3F891B990B0895CF92DF92EF92FF9219
:1003C0000F931F93CF93DF937C018A01EB016B01A5
:1003D000C40ED51ECC15DD0551F06991D701ED9104
:1003E000FC910190F081E02DC7010995F3CFC80180
:1003F000DF91CF911F910F91FF90EF90DF90CF9001
:10040000089520911D3F203240F021E030E0FC01B2
:100410002283338390E080E0089580911E3FE82F8F
:10042000F0E0E552F14C60838F5F80931E3F809334
:100430001D3F81E090E0089590E080E00895CF9323
:10044000DF9300D000D0CDB7DEB799838A837B835A
:100450006C8344E050E0BE016F5F7F4F80EA9FE312
:10046000ABDF2496CDBFDEBFDF91CF91089542E090
:1004700050E068E27BE983E69FE3DDCE282F30E0A1
:100480008631A8F4F901EA51F54680818F3F79F071
:1004900020533546D901EC91B0E2EB9FF0011124D5
:1004A000FC5F9085982381E019F480E008958FEF38
:1004B00008954F925F926F927F928F929F92AF9228
:1004C000BF92CF92DF92EF92FF921F93CF93DF9371
:1004D000D0E0C0E0412C512C3201CC24CA9483E7F7
:1004E000D82E8CE1E82EF12C812C912C5401839490
:1004F000D701C6010C2E04C0B695A7959795879590
:100500000A94D2F780FF12C08C2FB8DF882371F0D5
:10051000D501C4010C2E04C0880F991FAA1FBB1F50
:100520000A94D2F7482A592A6A2A7B2A2196C0328D
:10053000D105F1F6C301B201DF91CF911F91FF9078
:10054000EF90DF90CF90BF90AF909F908F907F9073
:100550006F905F904F900895CF93DF93863108F0AE
:100560003FC090E0DC01AA51B5462C912F3FC1F16C
:10057000FC01E053F546E08130E2E39FF0011124F5
:10058000FC5F611130C026833081232399F4EC0194
:10059000C654D5462881309741F12F3F31F17096EE
:1005A000E20FF11D3FB7F8942081611121C0277F30
:1005B00020833FBF8C559546FC019081992381F0A3
:1005C0008C919034D1F09038A9F0903149F4853075
:1005D00008F0880F9091010A809589238093010A81
:1005E000DF91CF9108952583CFCFF0E0E0E0DACF1F
:1005F0002860DECF10928006F3CF20E4823009F429
:1006000020E89091920A922359F36FB7F894909151
:10061000800A9E7F9093800A4091920A30E0209554
:10062000309550E02423352398ED94BF2093920A0F
:1006300090918E0A90FFFCCF9091800A91609093E8
:10064000800A813039F4809150048F778093500470
:100650006FBFC6CF809151048F7780935104F8CF3C
:10066000CF93DF93863198F490E0FC01EA51F54690
:1006700020812F3F61F0FC01E053F546E08130E23C
:10068000E39FF0011124FC5F613021F42183DF91AD
:10069000CF91089586549546EC01A881309791F04A
:1006A000AF3F81F0CF014096A80FB92FB11D9FB782
:1006B000F89422838C91623041F488608C93258376
:1006C0009FBFE5CFB0E0A0E0F2CF877F8C93268379
:1006D000F7CF7F928F929F92AF92BF92CF92DF928D
:1006E000EF92FF920F931F93CF93DF93EC014A0198
:1006F0005B01722E8D89882339F0E881F9810288A7
:10070000F389E02DCE0109950C8910E0E12C8114CC
:1007100088EB98068BE0A806B10430F0B694A79455
:100720009794879432E0E32E1D8AFFB6F894CC8428
:10073000DD8460E074EB84EC94E0A50194016CD955
:10074000F60120873187EC85FD857782EC85FD8574
:100750008681897F806C8E298683EC85FD858581E5
:1007600080688583909101028B8980958923980107
:10077000220F331F020F131FC00FD11F9889892B1F
:100780008093010262E08E856BDF61E08F8568DF18
:10079000FFBEDF91CF911F910F91FF90EF90DF90FF
:1007A000CF90BF90AF909F908F907F900895CF9201
:1007B000DF92EF92FF929091BB3E8091BC3E9111EF
:1007C0000FC067E8813009F44FC0823011F562ED47
:1007D00074E081E794E1FF90EF90DF90CF902FCE0F
:1007E0009130E1F4843011F464DEF5CF8A3089F47D
:1007F00060914A3F70914B3F80914C3F90914D3FAB
:100800001EDE10924A3F10924B3F10924C3F1092C6
:100810004D3FFF90EF90DF90CF900895993009F011
:100820006AC0873008F462C099EF980F4FEC50E02F
:100830006CE170E0092E04C0769567955795479551
:100840000A94D2F740FD17C081E08093BA3ECC24D1
:10085000CA94DC2CE12CF12C99278F2D7E2D6D2D47
:1008600080EA9FE3CEDD6C2D80EA9FE3FF90EF905E
:10087000DF90CF90C6CD97FD04C09430F8F49DEF83
:10088000980F892F8F77803258F580910006882342
:1008900051F19F719093060681E080930806809144
:1008A0000B0680FFFCCF80911006909111066C0121
:1008B000990FEE08FF081092BA3ECECF953171F035
:1008C00027EE280F233010F498EEDACF983010F48A
:1008D0009BEFD6CF9C30A8F29FEFD3CF90E0D1CF43
:1008E00083E090E8E4CF81E090E8E1CF811191CFFF
:1008F0006091BA3EB9CF9D3009F08BCF8F7790E0F1
:100900006FD8682FB1CFCF92DF92EF92FF92CF9343
:10091000DF93F894109213081092160881E08093E8
:10092000150810921C081092190810921E081092B7
:10093000293F8FE790E054D887FDADC08093003EFB
:1009400062E080E18DDE80E199DD811105C080915A
:10095000003E8F5F8093003E62E081E181DE81E1B5
:100960008DDD811105C08091003E8E5F8093003E39
:10097000D0E0C0E0CC24CA9433E7D32E3CE1E32E90
:10098000F12CD701C6010C2E04C0B695A7959795FA
:1009900087950A94D2F780FF06C060E08C2F60DE56
:1009A00060E08C2FD9DD2196C032D10551F710922D
:1009B0004E3F10924F3F1092503F1092513F109275
:1009C0004A3F10924B3F10924C3F10924D3F109275
:1009D000BA3E1092B93E90E080E0FC01ED5FF14C30
:1009E00010820196843B9105C1F71092B83E809128
:1009F000003E10921C3F10921B3F10921E3F10921F
:100A00001D3F9091293F911123C09091010294FFC5
:100A100044C096E09093060492E09093293F109290
:100A2000363F1092353F1092213F1092223F109294
:100A3000023E1092333F90E0880F991F80931C086C
:100A400010921E0881EE8093190881E080931308AC
:100A500080E991E080931F3F9093203F8BED9EE3D0
:100A600080932C3F90932D3F80EA91E08093313F1B
:100A70009093323F8BEF9EE380932E3F90932F3FD6
:100A800080E28093303F7894DF91CF91FF90EF9098
:100A9000DF90CF90089589E451CF93E0909326049E
:100AA000BBCF4F925F926F927F928F929F92AF9245
:100AB000BF92CF92DF92EF92FF920F931F93CF934B
:100AC000DF93CDB7DEB72B97CDBFDEBF9C013327B9
:100AD000203238F4F901E554F14C10822F5F3F4F7A
:100AE000F7CFAC01092E000C660B770B4132510594
:100AF0006105710508F082C07C01D12C0D2D10E03C
:100B00000E151F054CF480EA9FE33CDCF801E55428
:100B1000F14C8083D394F2CF8091BB3E1091BC3EC8
:100B2000811117C01F3709F069C02B96CDBFDEBFFA
:100B3000DF91CF911F910F91FF90EF90DF90CF90B9
:100B4000BF90AF909F908F907F906F905F904F90ED
:100B5000DACE813009F0BBC04091BD3E8091BE3EEF
:100B60003091BF3E2091C03EEEEFE10FEB3008F038
:100B700045C0F0E0E354FA4F09940AC009C03EC0F2
:100B800007C006C03BC004C039C038C001C000C0A7
:100B900090E0B0E0A0E0942BBA2FA92F982F8827DF
:100BA000832BBA2FA92F982F8827822B6C017D01C8
:100BB00023E7D2222CE1E222FF24B12CA12C412CEC
:100BC000512C32014394B701A6010A2C04C076953A
:100BD0006795579547950A94D2F740FF08C061E0A2
:100BE000123019F01330F1F460E08A2D39DD9FEFF7
:100BF000A91AB90AA0E2AA16B10429F72B96CDBF0B
:100C0000DEBFDF91CF911F910F91FF90EF90DF90AA
:100C1000CF90BF90AF909F908F907F906F905F909C
:100C20004F90089561E0153019F0163021F460E01E
:100C30008A2D92DCDCCF62E01B30B9F21C30A1F2CD
:100C4000183019F540914E3F50914F3F6091503F01
:100C50007091513FD301C2010A2C04C0880F991F23
:100C6000AA1FBB1F0A94D2F788879987AA87BB87D8
:100C7000842B952BA62BB72B80934E3F90934F3F01
:100C8000A093503FB093513FB2CF193009F0AFCF8E
:100C900040914E3F50914F3F6091503F7091513F76
:100CA000D301C2010A2C04C0880F991FAA1FBB1FC1
:100CB0000A94D2F788879987AA87BB8780959095F1
:100CC000A095B09584239523A623B723D5CF88304C
:100CD00009F0A0C10091BD3EC090BE3E2091BF3E34
:100CE000A02EB12C83E092E3A0E0B0E0002E04C07F
:100CF000B695A795979587950A94D2F780FD04C07D
:100D000081E08093B93E7ACFDC2CC22E113009F0FD
:100D1000B1C061E0802FA4DC0631A0F5F501EA51F5
:100D2000F54620812F3F71F1CD2CDD24F501EC55E6
:100D3000F54680818034B1F1803871F1803109F05D
:100D400093C0C114D10421F460E0802F05DC17C0EA
:100D5000BFEFCB16D10411F461E0F7CF2530A8F036
:100D6000220F822F847499F01C5FE12FF0E0E85D80
:100D7000F54FC0828091010A282B2093010A61E07F
:100D8000802F6EDC1092B93E39CF10E0EACF822F6F
:100D9000827259F31E5FE9CFC092810681E480938D
:100DA0008006EDCFC114D10409F440C0EFEFCE1698
:100DB000D10409F43CC010E03FB7F89480918E0A4A
:100DC00083708330D9F78FEF90E08C199D09880FDD
:100DD000991F0197223071F58093AC0A9093AD0A68
:100DE00080E89091920A982359F59091800A9E7F0D
:100DF0009093800A9091920A892B90E048ED44BF2D
:100E00008093920A80918E0A80FFFCCF8091800AA5
:100E100081608093800A11111DC02130A9F4809156
:100E200050048F77809350043FBFA9CF10E0D12C9E
:100E3000C12CC2CF8093A80A9093A90A80E4D1CF95
:100E400081E08093840AE7CF809151048F7780936B
:100E50005104EACF213021F4809150048068E2CF20
:100E6000809151048068F3CFF0E8CF16D10408F0E8
:100E700073CF6ACF123009F0C1CEC114D104A1F4EE
:100E80008091013E08137ECF1FB7F89410923D3F2A
:100E900010923E3F10923F3F1092403F8FEF809361
:100EA000013E01DA1FBF6ECF063108F06BCFF501AE
:100EB000EA51F54670802FEF721609F463CFF12CDA
:100EC000E12C4AE0C701B601ECD3D1DA60E07DE263
:100ED00081E391E0A7019601C1D569017A01F594FA
:100EE000E794D794C794C114D10481E0E806F104D3
:100EF0008CF41092400A10E091E0C916D104E90682
:100F0000F10468F0163059F0F694E794D794C7943A
:100F10001F5FF2CF82E08093400A1FEFEDCFA1E088
:100F2000CA1AD108E108F108C114D104B1E0EB06F6
:100F3000F10428F0CC24CA94DC2CE12CF12C4AE0FA
:100F4000C701B601AED393DA812E012E000C9908A9
:100F50001F3F09F058C06DE283E69FE326D94AE0BF
:100F600061E070E080E090E09CD381DA8091013E06
:100F70000817F9F08091013E8F3F41F0E091423F28
:100F8000F091433F8091413F31978083F501E053D9
:100F9000F546E08120E2E29FF0011124FC5F7092AF
:100FA000413FCF0107968093423F9093433F768223
:100FB00071828FEF9FEFDC0180933D3F90933E3F26
:100FC000A0933F3FB093403F81E090E001C0880F85
:100FD0008A94EAF78093443F8093453FC0924C0A3D
:100FE000D0924D0A1092410A10924A0A10924B0A6E
:100FF0000093013E81E08093450A8091400A816020
:101000008093400ABFCE612F110F770B880B990B8D
:101010004AE0AACF8E3009F0A1C0123009F4EECD1B
:10102000133079F48091BD3E9091BE3E8927982778
:101030008927853B910510F084EB90E08093B83EC2
:10104000DDCD113029F48091BD3E8093B73ED6CDE1
:10105000143001F56091BD3E8091BE3E762F682F21
:1010600090E084E0E81AF108292F30E02E153F05C2
:101070000CF0C4CDA901460F571F443B510540F465
:101080002154314CD9018C914D5F514CFA01808330
:101090009F5FEACF153009F0B1CD8091B73E61E096
:1010A000DFDA2091B83E30E0A091B73EA63108F0DB
:1010B00052C0EA2FF0E0E053F546E081E73008F057
:1010C0004AC080E2E89FF0011124FC5FBF016C5F21
:1010D0007F4F8FEFA63120F4B0E0AA51B5468C9136
:1010E00029833A839091033E9F839481982B9E831A
:1010F0009481809589238D838D818C8388E08B8377
:101100005E811D814F813B812C8189819A81FB0108
:10111000A4E0BEE3508347FD252F3A95000000C0B0
:101120002083212F49F0441F00C000C0108300001D
:1011300000C000C000C0EECF38E04D910000108329
:10114000000000C000C0019729F74F833B832C8328
:1011500089839A8353CDF0E0E0E0B8CF8D3009F079
:101160004DCD1F3FE1F493E0E916F104C1F4609125
:10117000BD3E8FE790E02B96CDBFDEBFDF91CF91D4
:101180001F910F91FF90EF90DF90CF90BF90AF90A5
:101190009F908F907F906F905F904F9029C400E058
:1011A000A2E0EA1AF108C12ED12C802F90E08E1512
:1011B0009F050CF023CD9601280F391F20383105EB
:1011C00044F48354914CFC016081812F800F90E0A6
:1011D0000FD40F5FEACF1F920F920FB60F92112418
:1011E0002F933F934F935F936F937F938F939F932F
:1011F000AF93BF93EF93FF9383E69FE397D7FF915E
:10120000EF91BF91AF919F918F917F916F915F917E
:101210004F913F912F910F900FBE0F901F901895F7
:101220001F920F920FB60F9211242F938F939F93BB
:10123000EF93FF93E0916F3FF091703F818181FDCB
:101240001BC090818091793F8F5F8F7020917A3F92
:10125000821741F0E091793FF0E0ED59F04C938F27
:101260008093793FFF91EF919F918F912F910F90F4
:101270000FBE0F901F9018958081F4CF1F920F9290
:101280000FB60F9211242F933F934F935F936F9359
:101290007F938F939F93AF93BF93EF93FF9380912F
:1012A000453F81508093453F81112BC08091443F41
:1012B0008093453FE091423FF091433F8091413F11
:1012C000808380913D3F90913E3FA0913F3FB09100
:1012D000403F181619061A061B063CF580913D3F43
:1012E00090913E3FA0913F3FB091403F0197A109AF
:1012F000B10980933D3F90933E3FA0933F3FB09311
:10130000403F81E08093460AFF91EF91BF91AF91FA
:101310009F918F917F916F915F914F913F912F910D
:101320000F900FBE0F901F90189580913D3F9091A8
:101330003E3FA0913F3FB091403F892B8A2B8B2BA2
:1013400001F7B1D7DECF1F920F920FB60F92112483
:101350002F933F934F935F936F938F939F93AF938D
:10136000BF9380915A3F90915B3FA0915C3FB091B9
:101370005D3F40913B3F50913C3F9A01205D3C4F87
:10138000283E63E0360728F0285E33400196A11D11
:10139000B11D20933B3F30933C3F80935A3F909345
:1013A0005B3FA0935C3FB0935D3F8091373F90914E
:1013B000383FA091393FB0913A3F0196A11DB11D30
:1013C0008093373F9093383FA093393FB0933A3F93
:1013D00081E080938D0ABF91AF919F918F916F9122
:1013E0005F914F913F912F910F900FBE0F901F90E3
:1013F00018951F920F920FB60F9211242F933F93BF
:101400004F935F936F937F938F939F93AF93BF930C
:10141000CF93EF93FF93C0911B08C2FF09C01092B6
:10142000363F1092353F1092343F84E04DD713C0C1
:10143000C6FF2DC08091333F813051F48091363FFB
:10144000E091313FF091323F90E009951092333FA7
:10145000C0FF13C046D7FF91EF91CF91BF91AF91DD
:101460009F918F917F916F915F914F913F912F91BC
:101470000F900FBE0F901F90189580E480931B086B
:1014800081E022D780911B0885FFE5CFE3CFC7FF1E
:101490005CC0C3FF08C01092363F1092353F1092D7
:1014A000343F83E0C3CF8091190880668093190888
:1014B00080911B0881FF2CC08091353F882349F023
:1014C00080911B0884FF05C082E080931A0881E0A8
:1014D000ADCF9091353F8091343F9817A8F4E091BB
:1014E000353F80912C3F90912D3FE80FF92FF11D52
:1014F000808180931D088091353F8F5F8093353FB9
:1015000083E080931A08A7CF82E080931A088ECFD9
:101510009091363F8091303F981790F480911D084C
:10152000E091363F20912E3F30912F3FE20FF32F75
:10153000F11D80838091363F8F5F8093363FE0CFEF
:1015400086E080931A0882E071CF85E06FCF1F920A
:101550000F920FB60F9211242F938F939F93EF93B7
:10156000FF9380911508982F9C70C1F08091150809
:1015700094E082FF93E090932B3F8093150881E0E5
:101580008093293F10922A3FFF91EF919F918F9175
:101590002F910F900FBE0F901F90189586FF46C099
:1015A0002091253F9091273F8091150884FF10C01E
:1015B0008091263F882351F083E08093140885E0D2
:1015C00080932B3F10922A3F81E02BC081E0F5CF22
:1015D0008091243F821760F4E091243FF0E0808105
:1015E000809318088091243F8F5F8093243FCCCF55
:1015F0008091283F891748F484E08093293F8091A7
:10160000233F816080931708BFCF8091263F8823B6
:1016100059F083E08093140881E080932B3F10926F
:101620002A3F8093293FB0CF81E0F4CF87FF37C0B6
:101630009091283F8091273F9817A8F4809118082F
:10164000E091283FF0E080838091283F8F5F809376
:10165000283F9091273F8091283F8917C0F482E06E
:101660008093140891CF8091263F882371F087E002
:101670008093140882E080932B3F10922A3F81E0F0
:101680008093293F1092273F7FCF85E0F1CF809153
:10169000263F882311F087E0BDCF85E0BBCF86E0F1
:1016A0008FCF8F929F92AF92BF920F931F93CF9342
:1016B000DF93CDB7DEB7A197CDBFDEBF19A2423011
:1016C00008F44AE08E010F5D1F4F842E912CB12C3F
:1016D000A12CA5019401A0D1E62FB901CA01EA30DD
:1016E00024F5E05DD801EE938D01232B242B252BCF
:1016F00081F790E080E0109761F0FD0101900020FB
:10170000E9F73197AF014A1B5B0BBD0183E69FE30D
:1017100092D5A196CDBFDEBFDF91CF911F910F91E2
:10172000BF90AF909F908F900895E95CDBCF1092AF
:10173000653F1092663F88EE93E0A0E0B0E08093B2
:10174000673F9093683FA093693FB0936A3F80E002
:101750009BE98093633F9093643F80E098E080939F
:101760006F3F9093703F88E08093713F89E0809352
:10177000723F1092733F83E18093743F82E18093C4
:10178000753F81E08093763F1092773F1092783FCB
:101790001092793F10927A3F10927B3F10927C3FDB
:1017A00087E180937D3F40E856E968E970E080E09A
:1017B0008F5F863011F487E00BC07695679557955B
:1017C0004795413099E059079DE36907710580F716
:1017D00094E090939F3F982F9095292F217030E0AF
:1017E00094E0220F331F9A95E1F78162822B809358
:1017F0009E3F1092A23F1092A33F88EE93E0A0E09C
:10180000B0E08093A43F9093A53FA093A63FB093F0
:10181000A73F8AE19BE98093A03F9093A13F089561
:1018200088ED90E084BF9093610083E58093020689
:101830008DE08093050680E28093030681E080932B
:1018400000068093030A9EEF9093260A9093270A3E
:101850009BE09093000A2DEF31E02093AE0A309385
:10186000AF0A1092810A90E89093820A80938C0AC2
:1018700081E18093800A789462E08FE0F1D643D8CA
:1018800081E595E08093463F9093473F87ED93E055
:101890008093483F9093493FCAE0F8940AD66093FA
:1018A000563F7093573F8093583F9093593F809194
:1018B000563F9091573FA091583FB091593F0091AA
:1018C000523F1091533F2091543F3091553F4091EA
:1018D0004E3F50914F3F6091503F7091513F802754
:1018E0009127A227B32784239523A623B723009704
:1018F000A105B10541F1C0935E3F00914E3F1091AB
:101900004F3F2091503F3091513F40914A3F50917D
:101910004B3F60914C3F70914D3F80239123A22318
:10192000B323842B952BA62BB72B80934A3F909300
:101930004B3FA0934C3FB0934D3F60E08FE00CD6FF
:1019400061E08FE08DD68091563F9091573FA091F6
:10195000583FB091593F8093523F9093533FA0938B
:10196000543FB093553F78942FB7F89480915A3FE5
:1019700090915B3FA0915C3FB0915D3F2FBF409144
:101980005F3F5091603F6091613F7091623F4817A7
:1019900059076A077B0781F080915E3F882329F011
:1019A00080915E3F815080935E3F80915E3F8111C8
:1019B00003C062E08FE054D62FB7F89480915A3F6D
:1019C00090915B3FA0915C3FB0915D3F2FBF8093B2
:1019D0005F3F9093603FA093613FB093623F5DCFC4
:1019E00003D0808199270895FC01E050FC4E0895B2
:1019F000262FAF93BF93E0E0F0E1328131FDFDCFC0
:101A0000DC01A050BC4E2C932DE924BF23E02083A1
:101A10000196BF91AF910895A1E21A2EAA1BBB1B9C
:101A2000FD010DC0AA1FBB1FEE1FFF1FA217B307AA
:101A3000E407F50720F0A21BB30BE40BF50B661FC0
:101A4000771F881F991F1A9469F760957095809584
:101A500090959B01AC01BD01CF010895052E97FB28
:101A600016F400940FD057FD05D0D6DF07FC02D046
:101A700046F408C050954095309521953F4F4F4F03
:101A80005F4F089590958095709561957F4F8F4F2A
:101A90009F4F0895EE0FFF1F0590F491E02D0994DC
:101AA000F894FFCF1010800000000000001010100C
:101AB0004040000000000000100004050607070673
:101AC00005040302010000010203040501020300F2
:101AD00000000000010101010101010102020202F6
:101AE0000202000000001020408080402010080406
:101AF0000201010204081020020408010000000095
:101B0000D5001B01410007017200500064004A012A
:101B100069037D004801000000000102DC011C0295
:101B2000B201D501C201B3010D0A0049FFFFFFFF59
:101B3000FFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFB5
:101B4000FFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFA5
:101B5000FFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFF95
:101B6000FFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFF85
:101B7000FFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFF75
:101B8000FFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFF65
:101B9000FFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFF55
:101BA000FFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFF45
:101BB000FFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFF35
:101BC000FFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFF25
:101BD000FFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFF15
:101BE000FFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFF05
:101BF000FFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFF5
:00000001FF
    )"}};

/*
 * Table of defined images
 */
const image_t *images[] = {
    &image_817,
};

uint8_t NUMIMAGES = sizeof(images) / sizeof(images[0]);
