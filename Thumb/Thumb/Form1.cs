using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Linq;
using System.Linq.Expressions;
using System.Text;
using System.Threading.Tasks;
using System.Windows.Forms;
using MySql.Data.MySqlClient;
using System.Security.Cryptography;
using MySql.Data;
using SourceAFIS;
using SourceAFIS.Simple;
using System.Threading;
using System.Runtime.InteropServices;
using System.IO;
using ScanAPIDemo;
using System.Drawing.Imaging;
using System.Windows.Media.Imaging;
using ScanAPIHelper;

using System.Runtime.Serialization;
using System.Runtime.Serialization.Formatters.Binary;



using System.Globalization;



namespace Thumb
{
    public partial class Form1 : Form
    {
        [Serializable]
        class MyFingerprint : Fingerprint
        {
            public string Filename;
            
        }

        // Inherit from Person in order to add Name field
        [Serializable]
        class MyPerson : Person
        {
            public string Name;
        }
        List<MyPerson> database = new List<MyPerson>();
        static MyPerson Enroll(string filename, string name)
        {
           // Console.WriteLine("Enrolling {0}...", name);

            // Initialize empty fingerprint object and set properties
            MyFingerprint fp = new MyFingerprint();
            fp.Filename = filename;
            // Load image from the file
            //Console.WriteLine(" Loading image from {0}...", filename);
            BitmapImage image = new BitmapImage(new Uri(filename, UriKind.RelativeOrAbsolute));
            fp.AsBitmapSource = image;
            // Above update of fp.AsBitmapSource initialized also raw image in fp.Image
            // Check raw image dimensions, Y axis is first, X axis is second
           // Console.WriteLine(" Image size = {0} x {1} (width x height)", fp.Image.GetLength(1), fp.Image.GetLength(0));

            // Initialize empty person object and set its properties
            MyPerson person = new MyPerson();
            person.Name = name;
            // Add fingerprint to the person
            person.Fingerprints.Add(fp);

            // Execute extraction in order to initialize fp.Template
            Console.WriteLine(" Extracting template...");
            Afis.Extract(person);
            // Check template size
            Console.WriteLine(" Template size = {0} bytes", fp.Template.Length);

            return person;
        }


        // Initialize path to images
        // static readonly string ImagePath = Path.Combine(Path.Combine("C:\\Users\\"), "images");
        static readonly string ImagePath = Path.Combine("C:"+"\\", "fingerprint");
        static readonly string FullPath = Path.GetFullPath("C:\\images");

        static AfisEngine Afis = new AfisEngine();

        delegate void SetTextCallback(string text);

        const int FTR_ERROR_EMPTY_FRAME = 4306; /* ERROR_EMPTY */
        const int FTR_ERROR_MOVABLE_FINGER = 0x20000001;
        const int FTR_ERROR_NO_FRAME = 0x20000002;
        const int FTR_ERROR_USER_CANCELED = 0x20000003;
        const int FTR_ERROR_HARDWARE_INCOMPATIBLE = 0x20000004;
        const int FTR_ERROR_FIRMWARE_INCOMPATIBLE = 0x20000005;
        const int FTR_ERROR_INVALID_AUTHORIZATION_CODE = 0x20000006;

        /* Other return codes are Windows-compatible */
        const int ERROR_NO_MORE_ITEMS = 259;                // ERROR_NO_MORE_ITEMS
        const int ERROR_NOT_ENOUGH_MEMORY = 8;              // ERROR_NOT_ENOUGH_MEMORY
        const int ERROR_NO_SYSTEM_RESOURCES = 1450;         // ERROR_NO_SYSTEM_RESOURCES
        const int ERROR_TIMEOUT = 1460;                     // ERROR_TIMEOUT
        const int ERROR_NOT_READY = 21;                     // ERROR_NOT_READY
        const int ERROR_BAD_CONFIGURATION = 1610;           // ERROR_BAD_CONFIGURATION
        const int ERROR_INVALID_PARAMETER = 87;             // ERROR_INVALID_PARAMETER
        const int ERROR_CALL_NOT_IMPLEMENTED = 120;         // ERROR_CALL_NOT_IMPLEMENTED
        const int ERROR_NOT_SUPPORTED = 50;                 // ERROR_NOT_SUPPORTED
        const int ERROR_WRITE_PROTECT = 19;                 // ERROR_WRITE_PROTECT
        const int ERROR_MESSAGE_EXCEEDS_MAX_SIZE = 4336;    // ERROR_MESSAGE_EXCEEDS_MAX_SIZE

        private Device m_hDevice;
        private bool m_bCancelOperation;
        private byte[] m_Frame;
        private bool m_bScanning;
        private byte m_ScanMode;
        private bool m_bIsLFDSupported;
        private bool m_bExit;



        public Form1()
        {
            InitializeComponent();
            m_hDevice = null;
            m_ScanMode = 0;
            m_bScanning = false;
            m_bExit = false;
           
           


            


        }

        private void closeToolStripMenuItem_Click(object sender, EventArgs e)
        {

        }
      

      
     
       
        private void button2_Click(object sender, EventArgs e)
        {
           

                if (textBox2.Text.Length > 0)
                {
                    String employeenum_substr = (textBox2.Text).Substring(4);
                    try
                    {
                        
                       

                        string tempimage = System.IO.Path.GetTempPath() + Guid.NewGuid().ToString() + textBox2.Text + ".bmp";

                        MyBitmapFile myFile = new MyBitmapFile(m_hDevice.ImageSize.Width, m_hDevice.ImageSize.Height, m_Frame);
                        FileStream file = new FileStream(tempimage, FileMode.Create);
                        file.Write(myFile.BitmatFileData, 0, myFile.BitmatFileData.Length);

                        //string tempimage = System.IO.Path.GetTempPath() + Guid.NewGuid().ToString() + ".csv";
                        BinaryFormatter formatter = new BinaryFormatter();
                        using (FileStream stream = File.OpenRead("database.dat"))
                            database = (List<MyPerson>)formatter.Deserialize(stream);
                        // Enroll visitor with unknown identity

                        //Instruction: use scanapihelper to get image to compare

                        MyPerson probe = Enroll(tempimage, "Visitor #12345");

                        // Look up the probe using Threshold = 10
                        Afis.Threshold = 45;
                        label3.Text = string.Format("Identifying {0} in database of {1} persons...", probe.Name, database.Count);
                        MyPerson match = Afis.Identify(probe, database).FirstOrDefault() as MyPerson;
                    // Null result means that there is no candidate with similarity score above threshold
                    float score = Afis.Verify(probe, match);
                    //Correct by breaking out of this if clause if dll keeps failing
                    if (match != null && score > Afis.Threshold)
                    {
                        // label3.Text = ("Probe {0} matches registered person {1}", probe.Name, match.Name);
                       
                        Console.WriteLine("Similarity score between {0} and {1} = {2:F3}", probe.Name, match.Name, score);
                        //close the file
                        //file.Close();
                        string[] serverconfig = System.IO.File.ReadAllLines("server.txt");
                       
                        MySqlConnection conn = new MySqlConnection(String.Format("server = {0}; database = {1}; uid = {2}; pwd = {3};", serverconfig[0], serverconfig[1], serverconfig[2], serverconfig[3]));
                        


                        conn.Open();

                        string validate = "SELECT employee FROM tabAttendance WHERE employee=" + "'EMP/" + textBox2.Text + "'" + "&& att_date = CURDATE()";
                        string validateemployee = "SELECT employee FROM tabEmployee WHERE employee=" + "'EMP/" + textBox2.Text + "'";
                        MySqlCommand val = new MySqlCommand(validate, conn);
                        MySqlCommand validateemployeeID = new MySqlCommand(validateemployee, conn);

                        MySqlDataAdapter dat2 = new MySqlDataAdapter(validateemployeeID);
                        DataTable tbl2 = new DataTable();
                        dat2.Fill(tbl2);

                        MySqlDataAdapter dat = new MySqlDataAdapter(val);
                        DataTable tbl = new DataTable();
                        
                        //string testemployeeID = "";
                        dat.Fill(tbl);
                        if (tbl.Rows.Count > 0 || tbl2.Rows.Count < 1)
                        {

                            label3.Text = "Failed! Employee must have been checked in or no valid employee selected; check ID entered";
                        }

                        else
                        {
                            string naming = String.Format("ATT-EMP/{0} {1}", textBox2.Text, DateTime.Now);
                            string upd = "INSERT INTO tabAttendance(name, naming_series, company, status, creation, modified, att_date, docstatus, fingerprint, employee, employee_name, time_in)  SELECT " + "'" + naming + "'" + ",'ATT-'," + "'PAIT Advanced Solutions'," + "'Present'" + ", NOW(), NOW(), CURDATE(), 1 ,fingerprint, employee, employee_name, NOW() FROM tabEmployee WHERE employee =" + "'EMP/" + textBox2.Text + "'" ;
                            MySqlCommand cmdd = new MySqlCommand(upd, conn);
                            cmdd.ExecuteNonQuery();
                            cmdd.Dispose();
                            label3.Text = "Attendance entered: SC:: " + score.ToString() + "  || THD = " + Afis.Threshold;

                        }
                        }else{ label3.Text = "Invalid employee or fingerprint data, try again"; }
                        // Print out any non-null result


                        // Compute similarity score




                    }
                    catch (Exception exc) when (exc is Exception || exc is ScanAPIException)
                    {

                        label3.Text = String.Format("failed with error: Retry thumbprinting {0}", exc.Message);
                    }
                }

                else
                {
                    label3.Text = "Enter valid Employee ID to continue";

                }
            
        }

        private void button3_Click(object sender, EventArgs e)
        {
            try
            {
               
                string[] serverconfig = System.IO.File.ReadAllLines("server.txt");
               
                MySqlConnection conn = new MySqlConnection(String.Format("server = {0}; database = {1}; uid = {2}; pwd = {3};", serverconfig[0], serverconfig[1], serverconfig[2], serverconfig[3]));

                conn.Open();
                if (conn.State == ConnectionState.Open)
                {
                    label3.Text = "Connected";
                }
                else { label3.Text = "Not Connected"; }


                String selectemp = "";
                
                selectemp = "EMP/"+textBox3.Text;

                if (string.IsNullOrEmpty(selectemp)) {

                    label3.Text = "Please enter an employee system ID";

                }

                else { 


                String comm = "SELECT employee_name, employee, department, branch FROM `tabEmployee` WHERE employee =" + "'" + selectemp + "'" ;
                MySqlCommand cmd = new MySqlCommand(comm, conn);


                MySqlDataAdapter dat = new MySqlDataAdapter(cmd);
                DataTable tbl = new DataTable();
                    
                    
                dat.Fill(tbl);
                    if (tbl.Rows.Count < 1){

                        label3.Text = "Invalid Employee selected";
                    }

                    else { dataGridView1.DataSource = tbl; }
                
                }

            }
            catch (Exception exc)
            {
                label3.Text = String.Format("failed with error: {0}", exc.Message);
            }
        }


        private void dataGridView_SelectionChanged(object sender, EventArgs e)
        {
            foreach (DataGridViewRow row in dataGridView1.SelectedRows)
            {
                string value1 = row.Cells[0].Value.ToString();
                string value2 = row.Cells[1].Value.ToString();
                //...
            }
        }
        private void GetFrame()
        {
            try
            {
                if (m_ScanMode == 0)
                    m_Frame = m_hDevice.GetFrame();
                else
                    m_Frame = m_hDevice.GetImage(m_ScanMode);
                label3.Text = ("OK");
            }
            catch (ScanAPIException ex)
            {
                if (m_Frame != null)
                    m_Frame = null;
                label3.Text = (ex.Message);
            }
        }
        private void CaptureThread()
        {
            m_bScanning = true;
            while (!m_bCancelOperation)
            {
                GetFrame();
                if (m_Frame != null)
                {
                    MyBitmapFile myFile = new MyBitmapFile(m_hDevice.ImageSize.Width, m_hDevice.ImageSize.Height, m_Frame);
                    MemoryStream BmpStream = new MemoryStream(myFile.BitmatFileData);
                    Bitmap Bmp = new Bitmap(BmpStream);
                    m_picture.Image = Bmp;


                }
                else
                    m_picture.Image = null;
                Thread.Sleep(10);
            }
            m_bScanning = false;
        }
        private void button1_Click(object sender, EventArgs e)
        {
            Afis = new AfisEngine();
            try
            {
               // String version = ScanAPIHelper.DiodesStatus.turn_off.ToString();

                String employeenum_substr =  (textBox2.Text).Substring(4);
            

                string[] serverconfig = System.IO.File.ReadAllLines("server.txt");
               
                MySqlConnection conn = new MySqlConnection(String.Format("server = {0}; database = {1}; uid = {2}; pwd = {3};", serverconfig[0], serverconfig[1], serverconfig[2], serverconfig[3]));

 

                conn.Open();

                RNGCryptoServiceProvider rnd = new RNGCryptoServiceProvider();
                byte[] random = new byte[256 * sizeof(int)];
                //byte[] random = new byte[4];
                rnd.GetBytes(random);
                uint code = BitConverter.ToUInt32(random, 0);
                String codex = Convert.ToString(code);


                string validate = "SELECT employee from tabEmployee WHERE employee = " + "'EMP/" + textBox2.Text + "'" +"&& fingerprint IS NOT NULL";

                MySqlCommand cmd = new MySqlCommand(validate, conn);


                MySqlDataAdapter dat = new MySqlDataAdapter(cmd);
                DataTable tbl = new DataTable();


              
                //database.Add(Enroll(Path.Combine(ImagePath, employeenum_substr + ".tif"), employeenum_substr));
                database.Add(Enroll(Path.Combine(ImagePath, textBox2.Text + ".bmp"), textBox2.Text));

                // Save the database to disk and load it back, just to try out the serialization
                BinaryFormatter formatter = new BinaryFormatter();
                label3.Text=("Saving database...");
                using (Stream stream = File.Open("database.dat", FileMode.Open, FileAccess.ReadWrite)) //FileMode.Create
                    formatter.Serialize(stream, database);

                dat.Fill(tbl);
            


                string employeeID = textBox2.Text;

                if (tbl.Rows.Count == 0 && employeeID != string.Empty )
                {string upd = "UPDATE tabEmployee SET fingerprint =" + "'" + codex + "', fpdata=" + "'"+ textBox2.Text + "'"  + " WHERE employee =" + "'EMP/" + textBox2.Text + "'";
                MySqlCommand cmdd = new MySqlCommand(upd, conn);
                cmdd.ExecuteNonQuery();
                cmdd.Dispose();

                    
                



                    label3.Text = "Successfully Enrolled; database updated" + database.ToString();

                    
                }

                else {   label3.Text = "Employee already enrolled"; }


              
              

            }
            catch (Exception exc) when (exc is Exception || exc is ScanAPIException)
            {

                label3.Text = String.Format("failed with error: {0}; Source {1}:: {2}", exc.Message, exc.Source, exc.InnerException);
            }

        }

        private void dataGridView1_CellContentClick(object sender, DataGridViewCellEventArgs e)
        {

        }

        private void button4_Click(object sender, EventArgs e)
        {
            try
            {
                if (passtext.Text == "1234")

                {
                    
                    string[] serverconfig = System.IO.File.ReadAllLines("server.txt");
                   
                    MySqlConnection conn = new MySqlConnection(String.Format("server = {0}; database = {1}; uid = {2}; pwd = {3};", serverconfig[0], serverconfig[1], serverconfig[2], serverconfig[3]));

                    conn.Open();
                    string upd = "UPDATE tabEmployee SET fpdata = NULL, fingerprint = null, fingerdata = NULL";
                    string upd_attendance = "TRUNCATE TABLE tabAttendance";
                    string upd_leave_allocation = "TRUNCATE TABLE `tabLeave Allocation`";


                    MySqlCommand cmdd1 = new MySqlCommand(upd_attendance, conn);
                    cmdd1.ExecuteNonQuery();
                    cmdd1.Dispose();

                    MySqlCommand cmdd2 = new MySqlCommand(upd_leave_allocation, conn);
                    cmdd2.ExecuteNonQuery();
                    cmdd2.Dispose();


                    MySqlCommand cmdd = new MySqlCommand(upd, conn);
                    cmdd.ExecuteNonQuery();
                    cmdd.Dispose();
                    label3.Text = "fingerdata cleared, attendance reset, absence reset!...";

                    BinaryFormatter formatter = new BinaryFormatter();
                    database.RemoveAll(x => x.Fingerprints != null);
                    //  label3.Text = ("Saving database...");
                    using (Stream stream = File.Open("database.dat", FileMode.Create, FileAccess.ReadWrite))
                        formatter.Serialize(stream, database);

                  


                }
                else
                {
                    label3.Text = "invalid passphrase provided";

                }
            }
            catch (Exception exc) { label3.Text = String.Format("An error occured:::: {0}", exc.Message); }
        }

        private void button5_Click(object sender, EventArgs e)
        {
            if (passtext.Text == "1234")
            {
                try
                {
                    
                    string[] serverconfig = System.IO.File.ReadAllLines("server.txt");
                    
                    MySqlConnection conn = new MySqlConnection(String.Format("server = {0}; database = {1}; uid = {2}; pwd = {3};", serverconfig[0], serverconfig[1], serverconfig[2], serverconfig[3]));

                    conn.Open();

                    string validate = "SELECT employee from tabEmployee";

                    MySqlCommand cmd = new MySqlCommand(validate, conn);
                    MySqlDataAdapter dt = new MySqlDataAdapter(cmd);


                    DataTable tbl = new DataTable();
                    dt.Fill(tbl);



                    //foreach (DataRow emp_id in tbl.Rows)
                    //  {


                    //get code


                    //  String[] filenames = Directory.GetFiles("C:\\fingerprint\\", "*.bmp").Select(Path.GetFileNameWithoutExtension());
                    var filenames = from fname in Directory.EnumerateFiles("C:\\fingerprint\\", "*.bmp")
                                    select (Path.GetFileNameWithoutExtension(fname));

                    //get a filename with path name containing employee data...
                    //var sel = tbl.AsEnumerable().Where(x => x.Field<String>("employee").Substring(4) == filenames.Any(f=>f.);
                    foreach (string nm in filenames)
                    {
                        RNGCryptoServiceProvider rnd = new RNGCryptoServiceProvider();
                        byte[] random = new byte[256 * sizeof(int)];
                        //byte[] random = new byte[4];
                        rnd.GetBytes(random);
                        uint code = BitConverter.ToUInt32(random, 0);
                        String codex = Convert.ToString(code);


                        var sel = tbl.AsEnumerable().Where(x => x.Field<String>("employee").Substring(4) == nm).First();

                        database.Add(Enroll(Path.Combine(ImagePath, sel.Field<String>("employee").Substring(4) + ".bmp"), sel.Field<String>("employee").Substring(4)));

                        // Save the database to disk and load it back, just to try out the serialization
                        BinaryFormatter formatter = new BinaryFormatter();
                        label3.Text = ("Saving database...");
                        using (Stream stream = File.Open("database.dat", FileMode.Open, FileAccess.ReadWrite))
                            formatter.Serialize(stream, database);


                        string upd = "UPDATE tabEmployee SET fingerprint =" + "'" + codex + "', fpdata = SUBSTRING(employee, 5) WHERE employee = '" + sel.Field<String>("employee") + "'";
                        MySqlCommand cmdd = new MySqlCommand(upd, conn);
                        cmdd.ExecuteNonQuery();

                        label3.Text = "Batch done";

                    }









                    //}





                }
                catch (Exception exc) when (exc is Exception || exc is ScanAPIException)
                {
                    label3.Text = String.Format("An error occured::: {0}::{1}::{2}::{3}", exc.Message, exc.Source, exc.InnerException, exc.Data);
                }
                catch (ScanAPIException scx)
                {

                    label3.Text += String.Format("An error occured::: {0}", scx.Message);
                }
            }
            else { label3.Text = "Invalid passphrase for high level operation"; }

        }

        private void button6_Click(object sender, EventArgs e)
        {
            try
            {
               
                string[] serverconfig = System.IO.File.ReadAllLines("server.txt");
               
                MySqlConnection conn = new MySqlConnection(String.Format("server = {0}; database = {1}; uid = {2}; pwd = {3};", serverconfig[0], serverconfig[1], serverconfig[2], serverconfig[3]));

                conn.Open();
                if (conn.State == ConnectionState.Open)
                {
                    label3.Text = "Connected";
                }
                else { label3.Text = "Not Connected"; }

                MySqlCommand cmd = new MySqlCommand("SELECT employee_name,employee,fingerprint,department,branch FROM `tabEmployee`", conn);


                MySqlDataAdapter dat = new MySqlDataAdapter(cmd);
                DataTable tbl = new DataTable();
                dat.Fill(tbl);

                dataGridView1.DataSource = tbl;

            }
            catch (Exception exc)
            {
                label3.Text = String.Format("Connection failed with error: {0}", exc.Message);
            }
        }

        private void button7_Click(object sender, EventArgs e)
        {
            m_hDevice = new Device();
            m_hDevice.Open(); 

            if (!m_bScanning)
            {
                m_bCancelOperation = false;
                button7.Text = "Stop";
                button2.Enabled = false;
               // m_btnClose.Enabled = false;
                Thread WorkerThread = new Thread(new ThreadStart(CaptureThread));
                WorkerThread.Start();
            }
            else
            {
                m_bCancelOperation = true;
                button7.Text = "Scan";
               // m_btnClose.Enabled = true;
                if (m_Frame != null)
                    button2.Enabled = true;
            }
        }

        private void button8_Click(object sender, EventArgs e)
        {
            try {
                var today = DateTime.Today;
                var endOfMonth = new DateTime(
                    today.Year,
                    today.Month,
                    DateTime.DaysInMonth(today.Year, today.Month),
                    23,
                    59,
                    59,
                    999
                );
                var firstDayOfMonth = new DateTime(today.Year, today.Month, 1);
                var lastDayOfMonth = firstDayOfMonth.AddMonths(1).AddTicks(-1);

                if (passtext.Text == "1234567" && ((endOfMonth - today).TotalDays < 3))
                {
                  
                    string[] serverconfig = System.IO.File.ReadAllLines("server.txt");
                   
                    MySqlConnection conn = new MySqlConnection(String.Format("server = {0}; database = {1}; uid = {2}; pwd = {3};", serverconfig[0], serverconfig[1], serverconfig[2], serverconfig[3]));

                    conn.Open();

                    string attendance = "SELECT * from tabAttendance";
                    string empl = "SELECT * from tabEmployee";

                    MySqlCommand cmd = new MySqlCommand(attendance, conn);
                    MySqlDataAdapter dt = new MySqlDataAdapter(cmd);

                    MySqlCommand cmd2 = new MySqlCommand(empl, conn);
                    MySqlDataAdapter dt2 = new MySqlDataAdapter(cmd);


                    DataTable attend_data = new DataTable();
                    dt.Fill(attend_data);

                    DataTable employee_data = new DataTable();
                    dt.Fill(employee_data);

                    var attend = attend_data.AsEnumerable();
                    var employee = employee_data.AsEnumerable();

                    foreach (DataRow x in employee)
                    {

                        RNGCryptoServiceProvider rnd = new RNGCryptoServiceProvider();
                        byte[] random = new byte[256 * sizeof(int)];
                        //byte[] random = new byte[4];
                        rnd.GetBytes(random);
                        uint code = BitConverter.ToUInt32(random, 0);
                        String codex = Convert.ToString(code);

                        int sum_attend = attend.Select(t => t.Field<string>("employee") == x.Field<string>("employee")).Count();


                        var WorkDaysInMonth = DateTime.DaysInMonth(DateTime.Today.Year, DateTime.Today.Month);

                        int leaves = WorkDaysInMonth - sum_attend;

                        // string upd = "INSERT INTO tabAttendance(name, naming_series, company, status, creation, modified, att_date, docstatus, fingerprint, employee, employee_name)  SELECT " + "'" + naming + "'" + ",'ATT-'," + "'PAIT Advanced Solutions'," + "'Present'" + ", NOW(), NOW(), CURDATE(), 1 ,fingerprint, employee, employee_name FROM tabEmployee WHERE employee =" + "'EMP/" + textBox2.Text + "'";

                        if (leaves > 0)
                        {

                            string updateLeave = "INSERT INTO `tabLeave Allocation` (name, creation, docstatus, leave_type, employee, employee_name, from_date, to_date, total_leaves_allocated) SELECT 'LAL/" + codex + "'" + ",NOW(), 1, 'Leave Without Pay', '" + x.Field<string>("employee") + "','" + x.Field<string>("employee_name") + "',DATE_SUB(curdate(), INTERVAL(DAY(curdate()) - 1) DAY), LAST_DAY(NOW())," + leaves;
                            // string updateLeave = "INSERT INTO `tabLeave Allocation` (name, creation, docstatus, leave_type, employee, employee_name, from_date, to_date, total_leaves_allocated) SELECT 'LAL//" + codex + "'" + ",NOW(), 1, 'Leave Without Pay', '" + x.Field<string>("employee") + "','" + x.Field<string>("employee_name") + "', DATE_SUB(curdate(),INTERVAL (DAY(curdate())-1) DAY),  " + "'" + endOfMonth + "'," + leaves;
                            MySqlCommand cmdd = new MySqlCommand(updateLeave, conn);
                            cmdd.ExecuteNonQuery();
                            cmdd.Dispose();

                        }

                        // int attended = 


                    }

                    label3.Text = "leaves and absence updated";

                }
                else { label3.Text = "Check passphrase. OR you're triggering absence entries too early. Do this within the last 3 days of the month"; }

            }
            catch (Exception cxv) { label3.Text = String.Format("An error occured::{0}", cxv.Message); }
        }

        private void button9_Click(object sender, EventArgs e)
        {

            try
            {
                string[] serverconfig = System.IO.File.ReadAllLines("server.txt");
               
                MySqlConnection conn = new MySqlConnection(String.Format("server = {0}; database = {1}; uid = {2}; pwd = {3};", serverconfig[0], serverconfig[1], serverconfig[2], serverconfig[3]));

               

                conn.Open();
                if (conn.State == ConnectionState.Open)
                {
                    label3.Text = "Connected";
                }
                else { label3.Text = "Not Connected"; }

                MySqlCommand cmd = new MySqlCommand("SELECT employee_name,employee,fingerprint,department,branch FROM `tabEmployee`", conn);


                MySqlDataAdapter dat = new MySqlDataAdapter(cmd);
                DataTable tbl = new DataTable();
                dat.Fill(tbl);

                dataGridView1.DataSource = tbl;

            }
            catch (Exception exc)
            {
                label3.Text = String.Format("Connection failed with error: {0}", exc.Message);
            }



        }

        private void button10_Click(object sender, EventArgs e)
        {
            try {

                string tempimage = System.IO.Path.GetTempPath() + Guid.NewGuid().ToString() + textBox2.Text + ".bmp";

                MyBitmapFile myFile = new MyBitmapFile(m_hDevice.ImageSize.Width, m_hDevice.ImageSize.Height, m_Frame);
                FileStream file = new FileStream(tempimage, FileMode.Create);
                file.Write(myFile.BitmatFileData, 0, myFile.BitmatFileData.Length);

                //string tempimage = System.IO.Path.GetTempPath() + Guid.NewGuid().ToString() + ".csv";
                BinaryFormatter formatter = new BinaryFormatter();
                using (FileStream stream = File.OpenRead("database.dat"))
                    database = (List<MyPerson>)formatter.Deserialize(stream);
                // Enroll visitor with unknown identity

                //Instruction: use scanapihelper to get image to compare

                MyPerson probe = Enroll(tempimage, "Visitor #12345");

                // Look up the probe using Threshold = 10
                Afis.Threshold = 45;
                label3.Text = string.Format("Identifying {0} in database of {1} persons...", probe.Name, database.Count);
                MyPerson match = Afis.Identify(probe, database).FirstOrDefault() as MyPerson;
                float score = Afis.Verify(probe, match);

                if (match != null && score > Afis.Threshold) {
                    // label3.Text = ("Probe {0} matches registered person {1}", probe.Name, match.Name);

                    //Console.WriteLine("Similarity score between {0} and {1} = {2:F3}", probe.Name, match.Name, score);
                    string[] serverconfig = System.IO.File.ReadAllLines("server.txt");
                 
                    MySqlConnection conn = new MySqlConnection(String.Format("server = {0}; database = {1}; uid = {2}; pwd = {3};", serverconfig[0], serverconfig[1], serverconfig[2], serverconfig[3]));

                  
                    conn.Open();

                string validate = "SELECT employee FROM tabAttendance WHERE employee=" + "'EMP/" + textBox2.Text + "'" + "&& att_date = CURDATE()";
                MySqlCommand validateemployeeID = new MySqlCommand(validate, conn);

             

                MySqlDataAdapter dat = new MySqlDataAdapter(validateemployeeID);
                DataTable tbl = new DataTable();

                //string testemployeeID = "";
                dat.Fill(tbl);

                if (tbl.Rows.Count < 1)
                {
                    label3.Text = "Employee did not sign in; sign out not allowed; this employee has been marked absent";


                }
                else {

                   
                        string clockout = "UPDATE tabAttendance SET time_out = NOW() WHERE employee = 'EMP/" + textBox2.Text + "'" + "&& att_date = CURDATE()";
                        MySqlCommand clockoutnow = new MySqlCommand(clockout, conn);

                        clockoutnow.ExecuteNonQuery();
                        clockoutnow.Dispose();

                        label3.Text = "Clock out completed: SC:: " + score.ToString() + " || TH = " + Afis.Threshold;
                    
                }

                }
                else { label3.Text = "Invalid employee or fingerprint data, try again"; }
            }

            catch (Exception cx) { label3.Text = String.Format("Clock out failed; fingerprint mismatch? retry scan. Internal error message:: {0}", cx.Message); }
        }
    }
}
