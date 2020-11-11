using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.IO;
using System.Security;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows.Forms;

namespace ACWW_Letter_Editor
{
    public partial class OpenFileDialogForm : Form
    {
        byte[] fileBytes;
        List<byte[]> letterStore_Plyr1_Inv = new List<byte[]>();
        List<byte[]> letterStore_Plyr2_Inv = new List<byte[]>();
        List<byte[]> letterStore_Plyr3_Inv = new List<byte[]>();
        List<byte[]> letterStore_Plyr4_Inv = new List<byte[]>();
        List<byte[]> letterStore_Plyr1_Mailbox = new List<byte[]>();
        List<byte[]> letterStore_Plyr2_Mailbox = new List<byte[]>();
        List<byte[]> letterStore_Plyr3_Mailbox = new List<byte[]>();
        List<byte[]> letterStore_Plyr4_Mailbox = new List<byte[]>();
        List<byte[]> letterStore_Plyr1_Bank = new List<byte[]>();
        List<byte[]> letterStore_Plyr2_Bank = new List<byte[]>();
        List<byte[]> letterStore_Plyr3_Bank = new List<byte[]>();
        List<byte[]> letterStore_Plyr4_Bank = new List<byte[]>();

        private static readonly uint[] letterSlots_Inv_Plyr1 = {
            0x00001158, 0x0000124C, 0x00001340, 0x00001434, 0x00001528,
            0x0000161C, 0x00001710, 0x00001804, 0x000018F8, 0x000019EC,
            0x00017138, 0x0001722C, 0x00017320, 0x00017320, 0x00017508,
            0x000175FC, 0x000176F0, 0x000177E4, 0x000178D8, 0x000179CC
        };

        private static readonly uint[] letterSlots_Inv_Plyr2 = {
            0x000033E4, 0x000034D8, 0x000035CC, 0x000036C0, 0x000037B4,
            0x000038A8, 0x0000399C, 0x00003A90, 0x00003B84, 0x00003C78,
            0x000193C4, 0x000194B8, 0x000195AC, 0x000196A0, 0x00019794,
            0x00019888, 0x0001997C, 0x00019A70, 0x00019B64, 0x00019C58
        };

        private static readonly uint[] letterSlots_Inv_Plyr3 = {
            0x00005670, 0x00005764, 0x00005858, 0x0000594C, 0x00005A40,
            0x00005B34, 0x00005C28, 0x00005D1C, 0x00005E10, 0x00005F04,
            0x0001B650, 0x0001B744, 0x0001B838, 0x0001B92C, 0x0001BA20,
            0x0001BB14, 0x0001BC08, 0x0001BCFC, 0x0001BDF0, 0x0001BEE4
        };

        private static readonly uint[] letterSlots_Inv_Plyr4 = {
            0x000078FC, 0x000079F0, 0x00007AE4, 0x00007BD8, 0x00007CCC,
            0x00007DC0, 0x00007EB4, 0x00007FA8, 0x0000809C, 0x00008190,
            0x0001D8DC, 0x0001D9D0, 0x0001DAC4, 0x0001DBB8, 0x0001DCAC,
            0x0001DDA0, 0x0001DE94, 0x0001DF88, 0x0001E07C, 0x0001E170
        };

        private static readonly uint[] letterSlots_mailBox_Plyr1 = {
            0x00012010, 0x00012104, 0x000121F8, 0x000122EC, 0x000123E0,
            0x000124D4, 0x000125C8, 0x000126BC, 0x000127B0, 0x000128A4,
            0x00027FF0, 0x000280E4, 0x000281D8, 0x000282CC, 0x000283C0,
            0x000284B4, 0x000285A8, 0x0002869C, 0x00028790, 0x00028884
        };

        private static readonly uint[] letterSlots_mailBox_Plyr2 = {
            0x0001299C, 0x00012A90, 0x00012B84, 0x00012C78, 0x00012D6C,
            0x00012E60, 0x00012F54, 0x00013048, 0x0001313C, 0x00013230,
            0x0002897C, 0x00028A70, 0x00028B64, 0x00028C58, 0x00028D4C,
            0x00028E40, 0x00028F34, 0x00029028, 0x0002911C, 0x00029210
        };

        private static readonly uint[] letterSlots_mailBox_Plyr3 = {
            0x00013328, 0x0001341C, 0x00013510, 0x00013604, 0x000136F8,
            0x000137EC, 0x000138E0, 0x000139D4, 0x00013AC8, 0x00013BBC,
            0x00029308, 0x000293FC, 0x000294F0, 0x000295E4, 0x000296D8,
            0x000297CC, 0x000298C0, 0x000299B4, 0x00029AA8, 0x00029B9C
        };

        private static readonly uint[] letterSlots_mailBox_Plyr4 = {
            0x00013CB4, 0x00013DA8, 0x00013E9C, 0x00013F90, 0x00014084,
            0x00014178, 0x0001426C, 0x00014360, 0x00014454, 0x00014548,
            0x00029C94, 0x00029D88, 0x00029E7C, 0x00029F70, 0x0002A064,
            0x0002A158, 0x0002A24C, 0x0002A340, 0x0002A434, 0x0002A528
        };

        private static readonly uint[] letterSlots_Bank_Plyr1 = {
            0x0002E210, 0x0002E304, 0x0002E3F8, 0x0002E4EC, 0x0002E5E0,
            0x0002E6D4, 0x0002E7C8, 0x0002E8BC, 0x0002E9B0, 0x0002EAA4,
            0x0002EB98, 0x0002EC8C, 0x0002ED80, 0x0002EE74, 0x0002EF68,
            0x0002F05C, 0x0002F150, 0x0002F244, 0x0002F338, 0x0002F42C,
            0x0002F520, 0x0002F614, 0x0002F708, 0x0002F7FC, 0x0002F8F0,
            0x0002F9E4, 0x0002FAD8, 0x0002FBCC, 0x0002FCC0, 0x0002FDB4,
            0x0002FEA8, 0x0002FF9C, 0x00030090, 0x00030184, 0x00030278,
            0x0003036C, 0x00030460, 0x00030554, 0x00030648, 0x0003073C,
            0x00030830, 0x00030924, 0x00030A18, 0x00030B0C, 0x00030C00,
            0x00030CF4, 0x00030DE8, 0x00030EDC, 0x00030FD0, 0x000310C4,
            0x000311B8, 0x000312AC, 0x000313A0, 0x00031494, 0x00031588,
            0x0003167C, 0x00031770, 0x00031864, 0x00031958, 0x00031A4C,
            0x00031B40, 0x00031C34, 0x00031D28, 0x00031E1C, 0x00031F10,
            0x00032004, 0x000320F8, 0x000321EC, 0x000322E0, 0x000323D4,
            0x000324C8, 0x000325BC, 0x000326B0, 0x000327A4, 0x00032898
        };

        private static readonly uint[] letterSlots_Bank_Plyr2 = {
            0x0003298C, 0x00032A80, 0x00032B74, 0x00032C68, 0x00032D5C,
            0x00032E50, 0x00032F44, 0x00033038, 0x0003312C, 0x00033220,
            0x00033314, 0x00033408, 0x000334FC, 0x000335F0, 0x000336E4,
            0x000337D8, 0x000338CC, 0x000339C0, 0x00033AB4, 0x00033BA8,
            0x00033C9C, 0x00033D90, 0x00033E84, 0x00033F78, 0x0003406C,
            0x00034160, 0x00034254, 0x00034348, 0x0003443C, 0x00034530,
            0x00034624, 0x00034718, 0x0003480C, 0x00034900, 0x000349F4,
            0x00034AE8, 0x00034BDC, 0x00034CD0, 0x00034DC4, 0x00034EB8,
            0x00034FAC, 0x000350A0, 0x00035194, 0x00035288, 0x0003537C,
            0x00035470, 0x00035564, 0x00035658, 0x0003574C, 0x00035840,
            0x00035934, 0x00035A28, 0x00035B1C, 0x00035C10, 0x00035D04,
            0x00035DF8, 0x00035EEC, 0x00035FE0, 0x000360D4, 0x000361C8,
            0x000362BC, 0x000363B0, 0x000364A4, 0x00036598, 0x0003668C,
            0x00036780, 0x00036874, 0x00036968, 0x00036A5C, 0x00036B50,
            0x00036C44, 0x00036D38, 0x00036E2C, 0x00036F20, 0x00037014
        };

        private static readonly uint[] letterSlots_Bank_Plyr3 = {
            0x00037108, 0x000371FC, 0x000372F0, 0x000373E4, 0x000374D8,
            0x000375CC, 0x000376C0, 0x000377B4, 0x000378A8, 0x0003799C,
            0x00037A90, 0x00037B84, 0x00037C78, 0x00037D6C, 0x00037E60,
            0x00037F54, 0x00038048, 0x0003813C, 0x00038230, 0x00038324,
            0x00038418, 0x0003850C, 0x00038600, 0x000386F4, 0x000387E8,
            0x000388DC, 0x000389D0, 0x00038AC4, 0x00038BB8, 0x00038CAC,
            0x00038DA0, 0x00038E94, 0x00038F88, 0x0003907C, 0x00039170,
            0x00039264, 0x00039358, 0x0003944C, 0x00039540, 0x00039634,
            0x00039728, 0x0003981C, 0x00039910, 0x00039A04, 0x00039AF8,
            0x00039BEC, 0x00039CE0, 0x00039DD4, 0x00039EC8, 0x00039FBC,
            0x0003A0B0, 0x0003A1A4, 0x0003A298, 0x0003A38C, 0x0003A480,
            0x0003A574, 0x0003A668, 0x0003A75C, 0x0003A850, 0x0003A944,
            0x0003AA38, 0x0003AB2C, 0x0003AC20, 0x0003AD14, 0x0003AE08,
            0x0003AEFC, 0x0003AFF0, 0x0003B0E4, 0x0003B1D8, 0x0003B2CC,
            0x0003B3C0, 0x0003B4B4, 0x0003B5A8, 0x0003B69C, 0x0003B790
        };

        private static readonly uint[] letterSlots_Bank_Plyr4 = {
            0x0003B884, 0x0003B978, 0x0003BA6C, 0x0003BB60, 0x0003BC54,
            0x0003BD48, 0x0003BE3C, 0x0003BF30, 0x0003C024, 0x0003C118,
            0x0003C20C, 0x0003C300, 0x0003C3F4, 0x0003C4E8, 0x0003C5DC,
            0x0003C6D0, 0x0003C7C4, 0x0003C8B8, 0x0003C9AC, 0x0003CAA0,
            0x0003CB94, 0x0003CC88, 0x0003CD7C, 0x0003CE70, 0x0003CF64,
            0x0003D058, 0x0003D14C, 0x0003D240, 0x0003D334, 0x0003D428,
            0x0003D51C, 0x0003D610, 0x0003D704, 0x0003D7F8, 0x0003D8EC,
            0x0003D9E0, 0x0003DAD4, 0x0003DBC8, 0x0003DCBC, 0x0003DDB0,
            0x0003DEA4, 0x0003DF98, 0x0003E08C, 0x0003E180, 0x0003E274,
            0x0003E368, 0x0003E45C, 0x0003E550, 0x0003E644, 0x0003E738,
            0x0003E82C, 0x0003E920, 0x0003EA14, 0x0003EB08, 0x0003EBFC,
            0x0003ECF0, 0x0003EDE4, 0x0003EED8, 0x0003EFCC, 0x0003F0C0,
            0x0003F1B4, 0x0003F2A8, 0x0003F39C, 0x0003F490, 0x0003F584,
            0x0003F678, 0x0003F76C, 0x0003F860, 0x0003F954, 0x0003FA48,
            0x0003FB3C, 0x0003FC30, 0x0003FD24, 0x0003FE18, 0x0003FF0C
        };

        private static readonly String[] letter_Backgrounds = {
            "./images/butterfly_paper.png", "./images/airmail_paper.png",       "./images/new_years_cards.png",     "./images/lacy_paper.png",      "./images/cloudy_paper.png",
            "./images/petal_paper.png",     "./images/snowy_paper.png",         "./images/maple_leaf_paper.png",    "./images/lined_paper.png",     "./images/notebook_paper.png",
            "./images/flowery_paper.png",   "./images/polka_dot_paper.png",     "./images/bottle_paper.png",        "./images/ribbon_paper.png",    "./images/sparkly_paper.png",
            "./images/vine_paper.png",      "./images/formal_paper.png",        "./images/snowman_paper.png",       "./images/card_paper.png",      "./images/leopard_paper.png",
            "./images/cow_paper.png",       "./images/camouflage_paper.png",    "./images/hamburger_paper.png",     "./images/piano_paper.png",     "./images/nook_paper.png",
            "./images/fox_paper.png",       "./images/birthday_cards.png",      "./images/four-leaf_paper.png",     "./images/town_hall_paper.png", "./images/tortimer_paper.png",
            "./images/insurance_paper.png", "./images/academy_paper.png",       "./images/lovely_paper.png",        "./images/rainbow_paper.png",   "./images/egyptian_paper.png",
            "./images/lotus_paper.png",     "./images/tile_paper.png",          "./images/mosaic_paper.png",        "./images/elegant_paper.png",   "./images/town_view_paper.png",
            "./images/chinese_paper.png",   "./images/ocean_paper.png",         "./images/industrial_paper.png",    "./images/fireworks_paper.png", "./images/floral_paper.png",
            "./images/mushroom_paper.png",  "./images/star_paper.png",          "./images/composer_paper.png",      "./images/bathtub_paper.png",   "./images/smb3_paper.png",
            "./images/cool_paper.png",      "./images/forest_paper.png",        "./images/bubble_paper.png",        "./images/buttercup_paper.png", "./images/tartan_paper.png",
            "./images/plaid_paper.png",     "./images/lemon-lime_paper.png",    "./images/crater_paper.png",        "./images/bejeweled_paper.png", "./images/geometric_paper.png",
            "./images/southwest_paper.png", "./images/night_sky_paper.png",     "./images/chic_paper.png",          "./images/goldfish_paper.png"
        };

        public OpenFileDialogForm()
        {
            InitializeComponent();
            ddl_PlayerSel.SelectedIndex = 0;
            ddl_ltrLocation.SelectedIndex = 0;
        }

        private void openSaveToolStripMenuItem_Click(object sender, EventArgs e)
        {
            selectButton_Click(sender, e);
        }

        private string decodeHex(byte[] convBytes)
        {
            String stringBuffer = "";
            for (int i = 0; i < convBytes.Length; i++)
            {
                stringBuffer += CharacterSets.WwCharacterDictionary[convBytes[i]];
            }

            stringBuffer = stringBuffer.Replace('\0',' ');
            return stringBuffer;
        }

        private void selectButton_Click(object sender, EventArgs e)
        {
            if (openFileDialog1.ShowDialog() == DialogResult.OK)
            {
                try
                {
                    var filePath = openFileDialog1.FileName;
                    using (Stream str = openFileDialog1.OpenFile())
                    {
                        using (BinaryReader binReader = new BinaryReader(File.OpenRead(filePath)))
                        { 
                            fileBytes = binReader.ReadBytes((int)str.Length);
                            binReader.Dispose();
                        }
                    }


                    //let's read the inventory letters of Player 1
                    for (int i = 0; i < letterSlots_Inv_Plyr1.Length; i++)
                    {
                        byte[] bytesLetter = new byte[0xF4];
                        Buffer.BlockCopy(fileBytes, (int)letterSlots_Inv_Plyr1[i], bytesLetter, 0, 0xF4);

                        letterStore_Plyr1_Inv.Add(bytesLetter);
                    }
                    //let's read the inventory letters of Player 2
                    for (int i = 0; i < letterSlots_Inv_Plyr2.Length; i++)
                    {
                        byte[] bytesLetter = new byte[0xF4];
                        Buffer.BlockCopy(fileBytes, (int)letterSlots_Inv_Plyr2[i], bytesLetter, 0, 0xF4);

                        letterStore_Plyr2_Inv.Add(bytesLetter);
                    }
                    //let's read the inventory letters of Player 3
                    for (int i = 0; i < letterSlots_Inv_Plyr4.Length; i++)
                    {
                        byte[] bytesLetter = new byte[0xF4];
                        Buffer.BlockCopy(fileBytes, (int)letterSlots_Inv_Plyr3[i], bytesLetter, 0, 0xF4);

                        letterStore_Plyr3_Inv.Add(bytesLetter);
                    }
                    //let's read the inventory letters of Player 4
                    for (int i = 0; i < letterSlots_Inv_Plyr4.Length; i++)
                    {
                        byte[] bytesLetter = new byte[0xF4];
                        Buffer.BlockCopy(fileBytes, (int)letterSlots_Inv_Plyr4[i], bytesLetter, 0, 0xF4);

                        letterStore_Plyr4_Inv.Add(bytesLetter);
                    }

                    //let's read the mailbox letters of Player 1
                    for (int i = 0; i < letterSlots_mailBox_Plyr1.Length; i++)
                    {
                        byte[] bytesLetter = new byte[0xF4];
                        Buffer.BlockCopy(fileBytes, (int)letterSlots_mailBox_Plyr1[i], bytesLetter, 0, 0xF4);

                        letterStore_Plyr1_Mailbox.Add(bytesLetter);
                    }
                    //let's read the mailbox letters of Player 2
                    for (int i = 0; i < letterSlots_mailBox_Plyr2.Length; i++)
                    {
                        byte[] bytesLetter = new byte[0xF4];
                        Buffer.BlockCopy(fileBytes, (int)letterSlots_mailBox_Plyr2[i], bytesLetter, 0, 0xF4);

                        letterStore_Plyr2_Mailbox.Add(bytesLetter);
                    }
                    //let's read the mailbox letters of Player 3
                    for (int i = 0; i < letterSlots_mailBox_Plyr3.Length; i++)
                    {
                        byte[] bytesLetter = new byte[0xF4];
                        Buffer.BlockCopy(fileBytes, (int)letterSlots_mailBox_Plyr3[i], bytesLetter, 0, 0xF4);

                        letterStore_Plyr3_Mailbox.Add(bytesLetter);
                    }
                    //let's read the mailbox letters of Player 4
                    for (int i = 0; i < letterSlots_mailBox_Plyr4.Length; i++)
                    {
                        byte[] bytesLetter = new byte[0xF4];
                        Buffer.BlockCopy(fileBytes, (int)letterSlots_mailBox_Plyr4[i], bytesLetter, 0, 0xF4);

                        letterStore_Plyr4_Mailbox.Add(bytesLetter);
                    }

                    //let's read the stored letters of Player 1
                    for (int i = 0; i < letterSlots_Bank_Plyr1.Length; i++)
                    {
                        byte[] bytesLetter = new byte[0xF4];
                        Buffer.BlockCopy(fileBytes, (int)letterSlots_Bank_Plyr1[i], bytesLetter, 0, 0xF4);

                        letterStore_Plyr1_Bank.Add(bytesLetter);
                    }
                    //let's read the stored letters of Player 2
                    for (int i = 0; i < letterSlots_Bank_Plyr1.Length; i++)
                    {
                        byte[] bytesLetter = new byte[0xF4];
                        Buffer.BlockCopy(fileBytes, (int)letterSlots_Bank_Plyr2[i], bytesLetter, 0, 0xF4);

                        letterStore_Plyr2_Bank.Add(bytesLetter);
                    }
                    //let's read the stored letters of Player 3
                    for (int i = 0; i < letterSlots_Bank_Plyr1.Length; i++)
                    {
                        byte[] bytesLetter = new byte[0xF4];
                        Buffer.BlockCopy(fileBytes, (int)letterSlots_Bank_Plyr3[i], bytesLetter, 0, 0xF4);

                        letterStore_Plyr3_Bank.Add(bytesLetter);
                    }
                    //let's read the stored letters of Player 4
                    for (int i = 0; i < letterSlots_Bank_Plyr1.Length; i++)
                    {
                        byte[] bytesLetter = new byte[0xF4];
                        Buffer.BlockCopy(fileBytes, (int)letterSlots_Bank_Plyr4[i], bytesLetter, 0, 0xF4);

                        letterStore_Plyr4_Bank.Add(bytesLetter);
                    }

                    refreshDDL();
                    loadLetterData();
                }
                catch (SecurityException ex)
                {
                    MessageBox.Show($"Security error.\n\nError message: {ex.Message}\n\n" +
                    $"Details:\n\n{ex.StackTrace}");
                }
            }
        }
        private void ddl_PlayerSel_SelectedIndexChanged(object sender, EventArgs e)
        {
            refreshDDL();
            loadLetterData();
        }

        private void ddl_ltrLocation_SelectedIndexChanged(object sender, EventArgs e)
        {
            refreshDDL();
            loadLetterData();
        }
        private void ddl_letterSelection_SelectedIndexChanged(object sender, EventArgs e)
        {
            loadLetterData();
        }

        private void refreshDDL()
        {
            int letters = 0;

            //ddl_letterSelection.Items.Clear();
            var ddlds_Letters = new List<Tuple<string, int>>();
            if (ddl_ltrLocation.SelectedIndex == 0)
                letters = 20;
            else if (ddl_ltrLocation.SelectedIndex == 1)
                letters = 20;
            else
                letters = 75;
            for(int i=0;i<letters;i++)
                ddlds_Letters.Add(Tuple.Create("Letter " + i, i));
            
            ddl_letterSelection.DataSource = ddlds_Letters;
        }

        private void loadLetterData()
        {
            List<byte[]> letterStore = new List<byte[]>();

            switch (ddl_PlayerSel.SelectedIndex)
            {
                case 0:
                    if (ddl_ltrLocation.SelectedIndex == 0)
                        letterStore = letterStore_Plyr1_Inv.ToList();
                    else if (ddl_ltrLocation.SelectedIndex == 1)
                        letterStore = letterStore_Plyr1_Mailbox.ToList();
                    else
                        letterStore = letterStore_Plyr1_Bank.ToList();
                    break;
                case 1:
                    if (ddl_ltrLocation.SelectedIndex == 0)
                        letterStore = letterStore_Plyr2_Inv.ToList();
                    else if (ddl_ltrLocation.SelectedIndex == 1)
                        letterStore = letterStore_Plyr1_Mailbox.ToList();
                    else
                        letterStore = letterStore_Plyr2_Bank.ToList();
                    break;
                case 2:
                    if (ddl_ltrLocation.SelectedIndex == 0)
                        letterStore = letterStore_Plyr3_Inv.ToList();
                    else if (ddl_ltrLocation.SelectedIndex == 1)
                        letterStore = letterStore_Plyr1_Mailbox.ToList();
                    else
                        letterStore = letterStore_Plyr3_Bank.ToList();
                    break;
                case 3:
                    if (ddl_ltrLocation.SelectedIndex == 0)
                        letterStore = letterStore_Plyr4_Inv.ToList();
                    else if (ddl_ltrLocation.SelectedIndex == 1)
                        letterStore = letterStore_Plyr1_Mailbox.ToList();
                    else
                        letterStore = letterStore_Plyr4_Bank.ToList();
                    break;
                default:
                    break;
            }

            if (!letterStore.Any())
                return;

            if (ddl_letterSelection.SelectedValue != null && ddl_letterSelection.SelectedValue is int && (int)ddl_letterSelection.SelectedValue != -1)
            {
                byte[] ltrSubject = new byte[0xF];
                byte[] ltrBody = new byte[0x63];
                byte[] ltrSignature = new byte[0x19];


                Buffer.BlockCopy(letterStore[(int)ddl_letterSelection.SelectedValue], 0x30, ltrSubject, 0, 0xF);
                Buffer.BlockCopy(letterStore[(int)ddl_letterSelection.SelectedValue], 0x48, ltrBody, 0, 0x63);
                Buffer.BlockCopy(letterStore[(int)ddl_letterSelection.SelectedValue], 0xC8, ltrSignature, 0, 0x19);


                pictureBox1.ImageLocation = letter_Backgrounds[int.Parse(letterStore[(int)ddl_letterSelection.SelectedValue][0xE9].ToString())];
                pictureBox1.Paint += new PaintEventHandler((sender, e) =>
                {
                    e.Graphics.Clear(SystemColors.Control);
                    e.Graphics.DrawImage(Image.FromFile(letter_Backgrounds[int.Parse(letterStore[(int)ddl_letterSelection.SelectedValue][0xE9].ToString())]), 0, 0);
                    e.Graphics.TextRenderingHint = System.Drawing.Text.TextRenderingHint.AntiAlias;
                    e.Graphics.DrawString(decodeHex(ltrSubject), Font, Brushes.Black, 50, 42);
                    e.Graphics.DrawString(decodeHex(ltrBody), Font, Brushes.Black, 50, 67);
                    e.Graphics.DrawString(decodeHex(ltrSignature), Font, Brushes.Black, 50, 137);
                    pictureBox1.Invalidate();
                });

                textBox1.Text = BitConverter.ToString(letterStore[(int)ddl_letterSelection.SelectedValue]);
                textBox2.Text = decodeHex(letterStore[(int)ddl_letterSelection.SelectedValue]);
            }
        }
    }
    public class ComboBoxObject
    {
        public ComboBoxObject(string Name, int Value)
        {
            this.Name = Name;
            this.Value = Value;
        }
        public string Name { get; set; }
        public int Value { get; set; }
    }

    internal static class CharacterSets
    {
        #region Wild World Character Sets

        // TODO: some of the new characters are wrong. Look at them one-by-one in Wild World somehow.
        internal static readonly string[] WwCharacterDictionary = {
            "\0", "A", "B", "C", "D", "E", "F", "G", "H", "I", "J", "K", "L", "M", "N", "O",
            "P", "Q", "R", "S", "T", "U", "V", "W", "X", "Y", "Z", "a", "b", "c", "d", "e",
            "f", "g", "h", "i", "j", "k", "l", "m", "n", "o", "p", "q", "r", "s", "t", "u",
            "v", "w", "x", "y", "z", "0", "1", "2", "3", "4", "5", "6", "7", "8", "9", "⨍",
            "s̊", "Œ", "Ž", "š", "œ", "ž", "Ÿ", "À", "Á", "Â", "Ã", "Ä", "Å", "Æ", "Ç", "È",
            "É", "Ê", "Ë", "Ì", "Í", "Î", "Ï", "Đ", "Ñ", "Ò", "Ó", "Ô", "Õ", "Ö", "Ø", "Ù",
            "Ú", "Û", "Ü", "Ý", "Þ", "ß", "à", "á", "â", "ã", "ä", "å", "æ", "ç", "è", "é",
            "ê", "ë", "ì", "í", "î", "ï", "ð", "ñ", "ò", "ó", "ô", "õ", "ö", "ø", "ù", "ú",
            "û", "ü", "ý", "þ", "ÿ", " ", "\n", "!", "“", "#", "$", "%", "&", "´", "(", ")",
            "*", "+", ",", "-", ".", "/", ":", ";", "<", "=", ">", "?", "@", "[", "{", "]",
            "|", "_", "}", "、", "˷", "…", "~", "£", "†", "‡", "^", "‰", "⟨", "`", "”", "•",
            "‒", "'", "—", "\"", "™", "⟩", " ", "˜", "¥", "╎", "§", "¡", "¢", "£", "¨", "©",
            "ª", "«", "¬", "–", "®", "°", "±", "²", "³", "‾", "ˢ", "µ", "¶", "→", "¹", "º",
            "»", "･", "¼", "½", "¾", "", "", "", "", "¿", "×", "÷", "💧", "★", "❤", "♪",
            "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "",
            "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "",
        };

        internal static readonly string[] WwJapaneseCharacterDictionary = {
            "\0", "あ", "い", "う", "え", "お", "か", "き", "く", "け", "こ", "さ", "し", "す", "せ", "そ",
            "た", "ち", "つ", "て", "と", "な", "に", "ぬ", "ね", "の", "は", "ひ", "ふ", "へ", "ほ", "ま",
            "み", "む", "め", "も", "や", "ゆ", "よ", "ら", "り", "る", "れ", "ろ", "わ", "を", "ん", "が",
            "ぎ", "ぐ", "げ", "ご", "ざ", "じ", "ず", "ぜ", "ぞ", "だ", "ぢ", "づ", "で", "ど", "ば", "び",
            "ぶ", "べ", "ぼ", "ぱ", "ぴ", "ぷ", "ぺ", "ぽ", "ぁ", "ぃ", "ぅ", "ぇ", "ぉ", "ゃ", "ゅ", "ょ",
            "っ", "ア", "イ", "ウ", "エ", "オ", "カ", "キ", "ク", "ケ", "コ", "サ", "シ", "ス", "セ", "ソ",
            "タ", "チ", "ツ", "テ", "ト", "ナ", "二", "ヌ", "ネ", "ノ", "ハ", "ヒ", "フ", "へ", "ホ", "マ",
            "ミ", "ム", "メ", "モ", "ヤ", "ユ", "ヨ", "ラ", "リ", "ル", "レ", "ロ", "ワ", "ヲ", "ソ", "ガ",
            "ギ", "グ", "ゲ", "ゴ", "ザ", "ジ", "ズ", "ゼ", "ゾ", "ダ", "ヂ", "ヅ", "デ", "ド", "バ", "ビ",
            "ブ", "ベ", "ボ", "パ", "ピ", "プ", "ペ", "ポ", "ァ", "ィ", "ゥ", "ェ", "ォ", "ャ", "ュ", "ョ",
            "ッ", "ヴ", "A", "B", "C", "D", "E", "F", "G", "H", "I", "J", "K", "L", "M", "N",
            "O", "P", "Q", "R", "S", "T", "U", "V", "W", "X", "Y", "Z", "a", "b", "c", "d",
            "e", "f", "g", "h", "i", "j", "k", "l", "m", "n", "o", "p", "q", "r", "s", "t",
            "u", "v", "w", "x", "y", "z", "0", "1", "2", "3", "4", "5", "6", "7", "8", "9",
            " ", "\n", "ー", "~", "･", "。", "、", "!", "?", ".", ",", "｢", "｣", "(", ")", "<",
            ">", "'", "\"", "_", "+", "=", "&", "@", ":", ";", "×", "÷", "🌢", "★", "♥", "♪"
        };

        #endregion
    }
}
