using System;
using System.Collections.Generic;
using System.Text;
using System.Runtime.InteropServices;

namespace ScanAPIDemo
{
    public class BITMAPFILEHEADER
    {
        private ushort bfType;
        private uint bfSize;
        private ushort bfReserved1;
        private ushort bfReserved2;
        private uint bfOffBits;

        public BITMAPFILEHEADER()
        {
            bfType = 'B' + 'M' * 0x100;
            bfReserved1 = bfReserved2 = 0;
        }
        public int SizeOfBFH
        {
            get
            {
                return (Marshal.SizeOf(bfType) + Marshal.SizeOf(bfSize) + Marshal.SizeOf(bfReserved1)
                            + Marshal.SizeOf(bfReserved2) + Marshal.SizeOf(bfOffBits));
            }
        }
        public uint BfSize
        {
            set { bfSize = value; }
        }
        public uint BfOffBits
        {
            set { bfOffBits = value; }
        }
        public byte[] GetByteData()
        {
            byte[] m_Data = new byte[SizeOfBFH];
            byte[] temp = System.BitConverter.GetBytes(bfType);
            int offset = 0;
            Array.Copy(temp, 0, m_Data, 0, temp.Length);
            offset = temp.Length;
            temp = System.BitConverter.GetBytes(bfSize);
            Array.Copy(temp, 0, m_Data, offset, temp.Length);
            offset += temp.Length;
            temp = System.BitConverter.GetBytes(bfReserved1);
            Array.Copy(temp, 0, m_Data, offset, temp.Length);
            offset += temp.Length;
            temp = System.BitConverter.GetBytes(bfReserved2);
            Array.Copy(temp, 0, m_Data, offset, temp.Length);
            offset += temp.Length;
            temp = System.BitConverter.GetBytes(bfOffBits);
            Array.Copy(temp, 0, m_Data, offset, temp.Length);
            return m_Data;
        }
    };

    public class RGBQUAD
    {
        private byte rgbBlue;
        private byte rgbGreen;
        private byte rgbRed;
        private byte rgbReserved;

        public RGBQUAD()
        {
            rgbReserved = 0;
        }
        public int SizeOfRgbquad
        {
            get
            {
                return (Marshal.SizeOf(rgbBlue) + Marshal.SizeOf(rgbGreen) + Marshal.SizeOf(rgbRed) + Marshal.SizeOf(rgbReserved));
            }
        }
        public byte RGBBlue
        {
            set { rgbBlue = value; }
        }
        public byte RGBGreen
        {
            set { rgbGreen = value; }
        }
        public byte RGBRed
        {
            set { rgbRed = value; }
        }
        public byte[] GetGRBTableByteData()
        {
            byte[] m_Data = new byte[256 * SizeOfRgbquad];
            int nOffset = 0;
            for (int i = 0; i < 256; i++)
            {
                m_Data[nOffset] = (byte)i;
                m_Data[nOffset + 1] = (byte)i;
                m_Data[nOffset + 2] = (byte)i;
                m_Data[nOffset + 3] = 0;
                nOffset += 4;
            }
            return m_Data;
        }
    };

    public class BITMAPINFOHEADER
    {
        private uint biSize;
        private int biWidth;
        private int biHeight;
        private ushort biPlanes;
        private ushort biBitCount;
        private uint biCompression;
        private uint biSizeImage;
        private int biXPelsPerMeter;
        private int biYPelsPerMeter;
        private uint biClrUsed;
        private uint biClrImportant;

        public BITMAPINFOHEADER()
        {
            biPlanes = 1;
            biBitCount = 8;
            biCompression = 0;  //BI_RGB; #define BI_RGB        0L
            biSizeImage = 0;
            biClrUsed = biClrImportant = 0;
            biXPelsPerMeter = 0x4CE6;	//500DPI
            biYPelsPerMeter = 0x4CE6;	//500DPI
            biSize = (uint)SizeOfBIH;
        }
        public int SizeOfBIH
        {
            get
            {
                return (Marshal.SizeOf(biSize) + Marshal.SizeOf(biWidth) + Marshal.SizeOf(biHeight) + Marshal.SizeOf(biPlanes)
                            + Marshal.SizeOf(biBitCount) + Marshal.SizeOf(biCompression) + Marshal.SizeOf(biSizeImage) + Marshal.SizeOf(biXPelsPerMeter)
                            + Marshal.SizeOf(biYPelsPerMeter) + Marshal.SizeOf(biClrUsed) + Marshal.SizeOf(biClrImportant));
            }
        }
        public uint BiSize
        {
            get { return biSize; }
            set { biSize = value; }
        }
        public int BiWidth
        {
            set { biWidth = value; }
        }
        public int BiHeight
        {
            set { biHeight = value; }
        }
        public int BiXPelsPerMeter
        {
            set { biXPelsPerMeter = value; }
        }
        public int BiYPelsPerMeter
        {
            set { biYPelsPerMeter = value; }
        }
        public byte[] GetByteData()
        {
            byte[] m_Data = new byte[SizeOfBIH];
            byte[] temp = System.BitConverter.GetBytes(biSize);
            int offset = 0;
            Array.Copy(temp, 0, m_Data, 0, temp.Length);
            offset = temp.Length;
            temp = System.BitConverter.GetBytes(biWidth);
            Array.Copy(temp, 0, m_Data, offset, temp.Length);
            offset += temp.Length;
            temp = System.BitConverter.GetBytes(biHeight);
            Array.Copy(temp, 0, m_Data, offset, temp.Length);
            offset += temp.Length;
            temp = System.BitConverter.GetBytes(biPlanes);
            Array.Copy(temp, 0, m_Data, offset, temp.Length);
            offset += temp.Length;
            temp = System.BitConverter.GetBytes(biBitCount);
            Array.Copy(temp, 0, m_Data, offset, temp.Length);
            offset += temp.Length;
            temp = System.BitConverter.GetBytes(biCompression);
            Array.Copy(temp, 0, m_Data, offset, temp.Length);
            offset += temp.Length;
            temp = System.BitConverter.GetBytes(biSizeImage);
            Array.Copy(temp, 0, m_Data, offset, temp.Length);
            offset += temp.Length;
            temp = System.BitConverter.GetBytes(biXPelsPerMeter);
            Array.Copy(temp, 0, m_Data, offset, temp.Length);
            offset += temp.Length;
            temp = System.BitConverter.GetBytes(biYPelsPerMeter);
            Array.Copy(temp, 0, m_Data, offset, temp.Length);
            offset += temp.Length;
            temp = System.BitConverter.GetBytes(biClrUsed);
            Array.Copy(temp, 0, m_Data, offset, temp.Length);
            offset += temp.Length;
            temp = System.BitConverter.GetBytes(biClrImportant);
            Array.Copy(temp, 0, m_Data, offset, temp.Length);
            return m_Data;
        }
    }

    public class BITMAPINFO
    {
        public BITMAPINFOHEADER bmiHeader;
        public RGBQUAD bmiColors;

        public BITMAPINFO()
        {
            bmiHeader = new BITMAPINFOHEADER();
            bmiColors = new RGBQUAD();
        }
        ~BITMAPINFO()
        {
            bmiHeader = null;
            bmiColors = null;
        }
        public int SizeOfBI
        {
            get { return (bmiHeader.SizeOfBIH + bmiColors.SizeOfRgbquad * 256); }
        }
    };

    public class MyBitmapFile
    {
        private BITMAPFILEHEADER m_fileHeaderBitmap;
        private BITMAPINFO m_infoBitmap;
        private byte[] m_BmpData;

        public MyBitmapFile()
        {
            m_fileHeaderBitmap = new BITMAPFILEHEADER();
            m_infoBitmap = new BITMAPINFO();
        }
        ~MyBitmapFile()
        {
            m_BmpData = null;
            m_fileHeaderBitmap = null;
            m_infoBitmap = null;
            GC.Collect();
        }

        public MyBitmapFile(int nWidth, int nHeight, byte[] pImage)
        {
            m_fileHeaderBitmap = new BITMAPFILEHEADER();
            m_infoBitmap = new BITMAPINFO();

            int length = m_fileHeaderBitmap.SizeOfBFH + m_infoBitmap.SizeOfBI + nWidth * nHeight;
            m_fileHeaderBitmap.BfSize = (uint)length;
            m_fileHeaderBitmap.BfOffBits = (uint)(m_fileHeaderBitmap.SizeOfBFH + m_infoBitmap.SizeOfBI);
            m_infoBitmap.bmiHeader.BiWidth = nWidth;
            m_infoBitmap.bmiHeader.BiHeight = nHeight;

            m_BmpData = new byte[length];
            byte[] TempData = m_fileHeaderBitmap.GetByteData();
            Array.Copy(TempData, 0, m_BmpData, 0, TempData.Length);
            int offset = TempData.Length;
            TempData = m_infoBitmap.bmiHeader.GetByteData();
            Array.Copy(TempData, 0, m_BmpData, offset, TempData.Length);
            offset += TempData.Length;
            TempData = m_infoBitmap.bmiColors.GetGRBTableByteData();
            Array.Copy(TempData, 0, m_BmpData, offset, TempData.Length);
            offset += TempData.Length;
            //rotate image
            byte[] pRotateImage = new byte[nWidth * nHeight];
            int nImgOffset = 0;
            for (int iCyc = 0; iCyc < nHeight; iCyc++)
            {
                Array.Copy(pImage, (nHeight - iCyc - 1) * nWidth, pRotateImage, nImgOffset, nWidth);
                nImgOffset += nWidth;
            }
            Array.Copy(pRotateImage, 0, m_BmpData, offset, pRotateImage.Length);
            TempData = null;
            pRotateImage = null;
        }

        public MyBitmapFile(int nWidth, int nHeight, IntPtr pImage)
        {
            m_fileHeaderBitmap = new BITMAPFILEHEADER();
            m_infoBitmap = new BITMAPINFO();

            int length = m_fileHeaderBitmap.SizeOfBFH + m_infoBitmap.SizeOfBI + nWidth * nHeight;
            m_fileHeaderBitmap.BfSize = (uint)length;
            m_fileHeaderBitmap.BfOffBits = (uint)(m_fileHeaderBitmap.SizeOfBFH + m_infoBitmap.SizeOfBI);
            m_infoBitmap.bmiHeader.BiWidth = nWidth;
            m_infoBitmap.bmiHeader.BiHeight = nHeight;

            m_BmpData = new byte[length];
            byte[] TempData = m_fileHeaderBitmap.GetByteData();
            Array.Copy(TempData, 0, m_BmpData, 0, TempData.Length);
            int offset = TempData.Length;
            TempData = m_infoBitmap.bmiHeader.GetByteData();
            Array.Copy(TempData, 0, m_BmpData, offset, TempData.Length);
            offset += TempData.Length;
            TempData = m_infoBitmap.bmiColors.GetGRBTableByteData();
            Array.Copy(TempData, 0, m_BmpData, offset, TempData.Length);
            offset += TempData.Length;
            //rotate image
            byte[] pRotateImage = new byte[nWidth * nHeight];
            int nImgOffset = 0;
            IntPtr pPtr;

            for (int iCyc = 0; iCyc < nHeight; iCyc++)
            {
                pPtr = (IntPtr)( pImage.ToInt32() +  (nHeight - iCyc - 1) * nWidth );
                Marshal.Copy(pPtr, pRotateImage, nImgOffset, nWidth);
                //Array.Copy(pImage, (nHeight - iCyc - 1) * nWidth, pRotateImage, nImgOffset, nWidth);
                nImgOffset += nWidth;
            }
            Array.Copy(pRotateImage, 0, m_BmpData, offset, pRotateImage.Length);
            TempData = null;
            pRotateImage = null;
        }

        public byte[] BitmatFileData
        {
            get
            {
                return m_BmpData;
            }
        }
    }
}
