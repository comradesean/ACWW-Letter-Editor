namespace ACWW_Letter_Editor
{
    partial class OpenFileDialogForm
    {
        /// <summary>
        /// Required designer variable.
        /// </summary>
        private System.ComponentModel.IContainer components = null;

        /// <summary>
        /// Clean up any resources being used.
        /// </summary>
        /// <param name="disposing">true if managed resources should be disposed; otherwise, false.</param>
        protected override void Dispose(bool disposing)
        {
            if (disposing && (components != null))
            {
                components.Dispose();
            }
            base.Dispose(disposing);
        }

        #region Windows Form Designer generated code

        /// <summary>
        /// Required method for Designer support - do not modify
        /// the contents of this method with the code editor.
        /// </summary>
        private void InitializeComponent()
        {
            System.ComponentModel.ComponentResourceManager resources = new System.ComponentModel.ComponentResourceManager(typeof(OpenFileDialogForm));
            this.menuStrip1 = new System.Windows.Forms.MenuStrip();
            this.fileToolStripMenuItem = new System.Windows.Forms.ToolStripMenuItem();
            this.openSaveToolStripMenuItem = new System.Windows.Forms.ToolStripMenuItem();
            this.openFileDialog1 = new System.Windows.Forms.OpenFileDialog();
            this.textBox1 = new System.Windows.Forms.TextBox();
            this.ddl_letterSelection = new System.Windows.Forms.ComboBox();
            this.ddl_PlayerSel = new System.Windows.Forms.ComboBox();
            this.ddl_ltrLocation = new System.Windows.Forms.ComboBox();
            this.textBox2 = new System.Windows.Forms.TextBox();
            this.pictureBox1 = new System.Windows.Forms.PictureBox();
            this.menuStrip1.SuspendLayout();
            ((System.ComponentModel.ISupportInitialize)(this.pictureBox1)).BeginInit();
            this.SuspendLayout();
            // 
            // menuStrip1
            // 
            this.menuStrip1.Items.AddRange(new System.Windows.Forms.ToolStripItem[] {
            this.fileToolStripMenuItem});
            this.menuStrip1.Location = new System.Drawing.Point(0, 0);
            this.menuStrip1.Name = "menuStrip1";
            this.menuStrip1.Size = new System.Drawing.Size(800, 24);
            this.menuStrip1.TabIndex = 0;
            this.menuStrip1.Text = "menuStrip1";
            // 
            // fileToolStripMenuItem
            // 
            this.fileToolStripMenuItem.DropDownItems.AddRange(new System.Windows.Forms.ToolStripItem[] {
            this.openSaveToolStripMenuItem});
            this.fileToolStripMenuItem.Name = "fileToolStripMenuItem";
            this.fileToolStripMenuItem.Size = new System.Drawing.Size(37, 20);
            this.fileToolStripMenuItem.Text = "File";
            // 
            // openSaveToolStripMenuItem
            // 
            this.openSaveToolStripMenuItem.Name = "openSaveToolStripMenuItem";
            this.openSaveToolStripMenuItem.Size = new System.Drawing.Size(130, 22);
            this.openSaveToolStripMenuItem.Text = "Open Save";
            this.openSaveToolStripMenuItem.Click += new System.EventHandler(this.openSaveToolStripMenuItem_Click);
            // 
            // openFileDialog1
            // 
            this.openFileDialog1.Filter = "Raw AC:WW Save File (*.sav)|*.sav";
            this.openFileDialog1.Title = "Open save file";
            // 
            // textBox1
            // 
            this.textBox1.Location = new System.Drawing.Point(12, 27);
            this.textBox1.Multiline = true;
            this.textBox1.Name = "textBox1";
            this.textBox1.Size = new System.Drawing.Size(383, 205);
            this.textBox1.TabIndex = 1;
            // 
            // ddl_letterSelection
            // 
            this.ddl_letterSelection.DisplayMember = "Item1";
            this.ddl_letterSelection.DropDownStyle = System.Windows.Forms.ComboBoxStyle.DropDownList;
            this.ddl_letterSelection.FormattingEnabled = true;
            this.ddl_letterSelection.Location = new System.Drawing.Point(404, 416);
            this.ddl_letterSelection.Name = "ddl_letterSelection";
            this.ddl_letterSelection.Size = new System.Drawing.Size(121, 21);
            this.ddl_letterSelection.TabIndex = 5;
            this.ddl_letterSelection.ValueMember = "Item2";
            this.ddl_letterSelection.SelectedIndexChanged += new System.EventHandler(this.ddl_letterSelection_SelectedIndexChanged);
            // 
            // ddl_PlayerSel
            // 
            this.ddl_PlayerSel.FormattingEnabled = true;
            this.ddl_PlayerSel.Items.AddRange(new object[] {
            "Player 1",
            "Player 2",
            "Player 3",
            "Player 4"});
            this.ddl_PlayerSel.Location = new System.Drawing.Point(404, 362);
            this.ddl_PlayerSel.Name = "ddl_PlayerSel";
            this.ddl_PlayerSel.Size = new System.Drawing.Size(121, 21);
            this.ddl_PlayerSel.TabIndex = 6;
            this.ddl_PlayerSel.SelectedIndexChanged += new System.EventHandler(this.ddl_PlayerSel_SelectedIndexChanged);
            // 
            // ddl_ltrLocation
            // 
            this.ddl_ltrLocation.FormattingEnabled = true;
            this.ddl_ltrLocation.Items.AddRange(new object[] {
            "Backpack",
            "Mailbox",
            "Bank Storage"});
            this.ddl_ltrLocation.Location = new System.Drawing.Point(404, 389);
            this.ddl_ltrLocation.Name = "ddl_ltrLocation";
            this.ddl_ltrLocation.Size = new System.Drawing.Size(121, 21);
            this.ddl_ltrLocation.TabIndex = 7;
            this.ddl_ltrLocation.SelectedIndexChanged += new System.EventHandler(this.ddl_ltrLocation_SelectedIndexChanged);
            // 
            // textBox2
            // 
            this.textBox2.Location = new System.Drawing.Point(13, 239);
            this.textBox2.Multiline = true;
            this.textBox2.Name = "textBox2";
            this.textBox2.Size = new System.Drawing.Size(383, 205);
            this.textBox2.TabIndex = 8;
            // 
            // pictureBox1
            // 
            this.pictureBox1.Location = new System.Drawing.Point(404, 27);
            this.pictureBox1.Name = "pictureBox1";
            this.pictureBox1.Size = new System.Drawing.Size(256, 256);
            this.pictureBox1.TabIndex = 9;
            this.pictureBox1.TabStop = false;
            // 
            // OpenFileDialogForm
            // 
            this.AutoScaleDimensions = new System.Drawing.SizeF(6F, 13F);
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
            this.ClientSize = new System.Drawing.Size(800, 450);
            this.Controls.Add(this.textBox2);
            this.Controls.Add(this.ddl_ltrLocation);
            this.Controls.Add(this.ddl_PlayerSel);
            this.Controls.Add(this.ddl_letterSelection);
            this.Controls.Add(this.textBox1);
            this.Controls.Add(this.menuStrip1);
            this.Controls.Add(this.pictureBox1);
            this.Icon = ((System.Drawing.Icon)(resources.GetObject("$this.Icon")));
            this.MainMenuStrip = this.menuStrip1;
            this.Name = "OpenFileDialogForm";
            this.Text = "ACWW Letter Editor";
            this.menuStrip1.ResumeLayout(false);
            this.menuStrip1.PerformLayout();
            ((System.ComponentModel.ISupportInitialize)(this.pictureBox1)).EndInit();
            this.ResumeLayout(false);
            this.PerformLayout();

        }

        #endregion

        private System.Windows.Forms.MenuStrip menuStrip1;
        private System.Windows.Forms.ToolStripMenuItem fileToolStripMenuItem;
        private System.Windows.Forms.ToolStripMenuItem openSaveToolStripMenuItem;
        private System.Windows.Forms.OpenFileDialog openFileDialog1;
        private System.Windows.Forms.TextBox textBox1;
        private System.Windows.Forms.ComboBox ddl_letterSelection;
        private System.Windows.Forms.ComboBox ddl_PlayerSel;
        private System.Windows.Forms.ComboBox ddl_ltrLocation;
        private System.Windows.Forms.TextBox textBox2;
        private System.Windows.Forms.PictureBox pictureBox1;
    }
}