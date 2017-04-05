// ScanAPIHelper.h
#pragma once

using namespace System;
using namespace System::Text;
using namespace System::Globalization;
using namespace System::Collections;
using namespace System::Drawing;

namespace ScanAPIHelper {

    public enum class FTRSCAN_INTERFACE_STATUS
    {
	    FTRSCAN_INTERFACE_STATUS_CONNECTED,
	    FTRSCAN_INTERFACE_STATUS_DISCONNECTED
    };

    public value struct DeviceInfo
    {
        Byte    DeviceCompatibility;
        Size    imageSize;

        static initonly DeviceInfo Empty = { (Size::Empty, 0 ) };
    };

    public value struct Version
    {
	    Int16       wMajorVersionHi; 
	    Int16       wMajorVersionLo; 
	    Int16       wMinorVersionHi; 
	    Int16       wMinorVersionLo;

        virtual String^ ToString() override
        {
            StringBuilder^ version = gcnew StringBuilder();
            if( wMajorVersionHi != -1 )
            {
                version->Append( wMajorVersionHi.ToString( CultureInfo::InvariantCulture->NumberFormat ) );

                if( wMajorVersionLo != -1 )
                {
                    version->Append( "." );
                    version->Append( wMajorVersionLo.ToString( CultureInfo::InvariantCulture->NumberFormat ) );

                    if( wMinorVersionHi != -1 )
                    {
                        version->Append( "." );
                        version->Append( wMinorVersionHi.ToString( CultureInfo::InvariantCulture->NumberFormat ) );

                        if( wMinorVersionLo != -1 )
                        {
                            version->Append( "." );
                            version->Append( wMinorVersionLo.ToString( CultureInfo::InvariantCulture->NumberFormat ) );
                        }
                    }
                }
            }
            return version->ToString();
        }

        static initonly Version Empty = { (0, 0, 0, 0 ) };
    };

    public value struct VersionInfo
    {
	    Version     APIVersion;
	    Version     HardwareVersion;
	    Version     FirmwareVersion;

        static initonly VersionInfo Empty = { (Version::Empty, Version::Empty, Version::Empty, Version::Empty ) };
    };

    [FlagsAttribute]
    public enum class LogMask : int
    {
        off = FTR_LOG_MASK_OFF,
        to_file = FTR_LOG_MASK_TO_FILE,
        to_aux = FTR_LOG_MASK_TO_AUX,
        timestamp = FTR_LOG_MASK_TIMESTAMP
    };

    public enum class LogLevel : int
    {
        minimum = FTR_LOG_LEVEL_MIN,
        optimal = FTR_LOG_LEVEL_OPTIMAL,
        full = FTR_LOG_LEVEL_FULL
    };

    public enum class DiodesStatus : Byte
    {
        turn_off = 0,
        turn_on_period  = 1,
        turn_on_permanent = 255
    };

	public value struct WsqImageParameter
	{
		Int32	Width;       // image Width
		Int32	Height;      // image Height
		Int32	DPI;         // resolution Dots per inch
		Int32	RAW_size;    // size of RAW image
		Int32	BMP_size;    // size of BMP image
		Int32	WSQ_size;    // size of WSQ image
		float   Bitrate;	 // compression
	    static initonly WsqImageParameter Empty = { (0, 0, 0, 0, 0, 0) };
	};

    public ref class Device : IDisposable
	{
    public:

        Device()
        {
            m_bDispose = false;
            m_hDevice = NULL;
            m_LastErrorCode = 0;
        }

        ///<summary>
        /// Releases the unmanaged resources used by the Device.
        ///</summary>
        virtual ~Device()
        {
            if( m_bDispose )
                return;

            this->!Device();
            m_bDispose = true;

            GC::SuppressFinalize( this );
        }

        ///<summary>
        /// opens device on the default interface
        ///</summary>
        ///<remarks>
        /// You can change the default interface number by calling <c>SetBaseInterface</c> function.
        ///</remarks>
        ///<exception cref="ScanAPIException">The operation fails. To get extended error information, see error code property</exception>
        ///<exception cref="ObjectDisposedException">The class instance is disposed. Any calls are prohibited.</exception>
        ///<exception cref="InvalidOperationException">The device is already opened.</exception>
        void Open()
        {
            CheckDispose();
            if( m_hDevice != NULL )
            {
                throw gcnew InvalidOperationException();
            }

            m_LastErrorCode = 0;
            m_hDevice = ftrScanOpenDevice();
            if( m_hDevice == NULL )
            {
                m_LastErrorCode = (int)GetLastError();
                throw gcnew ScanAPIException( m_LastErrorCode );
            }
        }

        ///<summary>
        /// opens device on the selected interface.
        ///</summary>
        ///<remarks>
        /// You can find the connected devices by calling the <c>GetInterfaces</c> function.
        ///</remarks>
        ///<param name='interfaceNumber'>Index of the device. The maximum number of devices is 128.
        /// The value must be between 0 and 127</param>
        ///<exception cref="ScanAPIException">The operation fails. To get extended error information, see error code property</exception>
        ///<exception cref="ObjectDisposedException">The class instance is disposed. Any calls are prohibited.</exception>
        ///<exception cref="InvalidOperationException">The device is already opened.</exception>
        ///<exception cref="ArgumentOutOfRangeException">The <c>interfaceNumber</c> parameter is out of range.</exception>
        void Open( int interfaceNumber )
        {
            CheckDispose();
            if( m_hDevice != NULL )
            {
                throw gcnew InvalidOperationException();
            }
            if( interfaceNumber >= FTR_MAX_INTERFACE_NUMBER || interfaceNumber < 0 )
            {
                throw gcnew ArgumentOutOfRangeException( "interfaceNumber" );
            }

            m_LastErrorCode = 0;
            m_hDevice = ftrScanOpenDeviceOnInterface( interfaceNumber );
            if( m_hDevice == NULL )
            {
                m_LastErrorCode = (int)GetLastError();
                throw gcnew ScanAPIException( m_LastErrorCode );
            }
        }

        ///<summary>
        /// Gets the current version number of the API, the device hardware version 
        /// and the device firmware version.
        ///</summary>
        ///<exception cref="ScanAPIException">The operation fails. To get extended error information, see error code property</exception>
        ///<exception cref="InvalidOperationException">The device is not opened.</exception>
        void Close()
        {
            CheckDispose();
            if( m_hDevice == NULL )
            {
                throw gcnew InvalidOperationException();
            }
        }

        ///<summary>
        /// Gets the current version number of the API, the device hardware version 
        /// and the device firmware version.
        ///</summary>
        ///<exception cref="ScanAPIException">The operation fails. To get extended error information, see error code property</exception>
        ///<exception cref="ObjectDisposedException">The class instance is disposed. Any calls are prohibited.</exception>
        ///<exception cref="InvalidOperationException">The device is not opened.</exception>
        property VersionInfo VersionInformation
        {
            VersionInfo get()
            {
                CheckDispose();
                if( m_hDevice == NULL )
                {
                    throw gcnew InvalidOperationException();
                }

                m_LastErrorCode = 0;
                FTRSCAN_VERSION_INFO info;
                memset( &info, 0, sizeof( FTRSCAN_VERSION_INFO ) );
                info.dwVersionInfoSize = sizeof( FTRSCAN_VERSION_INFO );
                if( !ftrScanGetVersion( m_hDevice, &info ) )
                {
                    m_LastErrorCode = (int)GetLastError();
                    throw gcnew ScanAPIException( m_LastErrorCode );
                }
                VersionInfo vinfo;

                vinfo.APIVersion.wMajorVersionHi = info.APIVersion.wMajorVersionHi;
                vinfo.APIVersion.wMajorVersionLo = info.APIVersion.wMajorVersionLo;
                vinfo.APIVersion.wMinorVersionHi = info.APIVersion.wMinorVersionHi;
                vinfo.APIVersion.wMinorVersionLo = info.APIVersion.wMinorVersionLo;

                vinfo.FirmwareVersion.wMajorVersionHi = info.FirmwareVersion.wMajorVersionHi;
                vinfo.FirmwareVersion.wMajorVersionLo = info.FirmwareVersion.wMajorVersionLo;
                vinfo.FirmwareVersion.wMinorVersionHi = info.FirmwareVersion.wMinorVersionHi;
                vinfo.FirmwareVersion.wMinorVersionLo = info.FirmwareVersion.wMinorVersionLo;

                vinfo.HardwareVersion.wMajorVersionHi = info.HardwareVersion.wMajorVersionHi;
                vinfo.HardwareVersion.wMajorVersionLo = info.HardwareVersion.wMajorVersionLo;
                vinfo.HardwareVersion.wMinorVersionHi = info.HardwareVersion.wMinorVersionHi;
                vinfo.HardwareVersion.wMinorVersionLo = info.HardwareVersion.wMinorVersionLo;

                return vinfo;
            }
        }

        ///<summary>
        /// Activates\Deactivates Live Finger Detection (LFD) feature during the capture process.
        ///</summary>
        ///<exception cref="ScanAPIException">The operation fails. To get extended error information, see error code property</exception>
        ///<exception cref="ObjectDisposedException">The class instance is disposed. Any calls are prohibited.</exception>
        ///<exception cref="InvalidOperationException">The device is not opened.</exception>
        property bool DetectFakeFinger
        {
            bool get()
            {
                CheckDispose();
                if( m_hDevice == NULL )
                {
                    throw gcnew InvalidOperationException();
                }

                m_LastErrorCode = 0;
                FTR_DWORD dwFlags = 0;
                if( !ftrScanGetOptions( m_hDevice, &dwFlags ) )
                {
                    m_LastErrorCode = (int)GetLastError();
                    throw gcnew ScanAPIException( m_LastErrorCode );
                }

                return ((dwFlags & FTR_OPTIONS_DETECT_FAKE_FINGER) != 0);
            }

            void set( bool value )
            {
                CheckDispose();
                if( m_hDevice == NULL )
                {
                    throw gcnew InvalidOperationException();
                }
                m_LastErrorCode = 0;
                if( !ftrScanSetOptions( m_hDevice, FTR_OPTIONS_DETECT_FAKE_FINGER, value ? FTR_OPTIONS_DETECT_FAKE_FINGER : 0 ) )
                {
                    m_LastErrorCode = (int)GetLastError();
                    throw gcnew ScanAPIException( m_LastErrorCode );
                }
            }
        }

		///<summary>
        /// Activates\Deactivates 'Invert Image' feature during the capture process.
        ///</summary>
        ///<exception cref="ScanAPIException">The operation fails. To get extended error information, see error code property</exception>
        ///<exception cref="ObjectDisposedException">The class instance is disposed. Any calls are prohibited.</exception>
        ///<exception cref="InvalidOperationException">The device is not opened.</exception>
        property bool InvertImage
        {
            bool get()
            {
                CheckDispose();
                if( m_hDevice == NULL )
                {
                    throw gcnew InvalidOperationException();
                }

                m_LastErrorCode = 0;
                FTR_DWORD dwFlags = 0;
                if( !ftrScanGetOptions( m_hDevice, &dwFlags ) )
                {
                    m_LastErrorCode = (int)GetLastError();
                    throw gcnew ScanAPIException( m_LastErrorCode );
                }

                return ((dwFlags & FTR_OPTIONS_INVERT_IMAGE) != 0);
            }

            void set( bool value )
            {
                CheckDispose();
                if( m_hDevice == NULL )
                {
                    throw gcnew InvalidOperationException();
                }
                m_LastErrorCode = 0;
                if( !ftrScanSetOptions( m_hDevice, FTR_OPTIONS_INVERT_IMAGE, value ? FTR_OPTIONS_INVERT_IMAGE : 0 ) )
                {
                    m_LastErrorCode = (int)GetLastError();
                    throw gcnew ScanAPIException( m_LastErrorCode );
                }
            }
        }

        ///<summary>
        /// Activates\Deactivates fast finger detection method
        ///</summary>
        ///<exception cref="ScanAPIException">The operation fails. To get extended error information, see error code property</exception>
        ///<exception cref="ObjectDisposedException">The class instance is disposed. Any calls are prohibited.</exception>
        ///<exception cref="InvalidOperationException">The device is not opened.</exception>
        property bool FastFingerDetectMethod
        {
            bool get()
            {
                CheckDispose();
                if( m_hDevice == NULL )
                {
                    throw gcnew InvalidOperationException();
                }

                m_LastErrorCode = 0;
                FTR_DWORD dwFlags = 0;
                if( !ftrScanGetOptions( m_hDevice, &dwFlags ) )
                {
                    m_LastErrorCode = (int)GetLastError();
                    throw gcnew ScanAPIException( m_LastErrorCode );
                }

                return ((dwFlags & FTR_OPTIONS_FAST_FINGER_DETECT_METHOD) != 0);
            }

            void set( bool value )
            {
                CheckDispose();
                if( m_hDevice == NULL )
                {
                    throw gcnew InvalidOperationException();
                }
                m_LastErrorCode = 0;
                if( !ftrScanSetOptions( m_hDevice, FTR_OPTIONS_FAST_FINGER_DETECT_METHOD, value ? FTR_OPTIONS_FAST_FINGER_DETECT_METHOD : 0 ) )
                {
                    m_LastErrorCode = (int)GetLastError();
                    throw gcnew ScanAPIException( m_LastErrorCode );
                }
            }
        }

        ///<summary>
        /// Increases\Decreases the image size and pixel size
        ///</summary>
        ///<exception cref="ScanAPIException">The operation fails. To get extended error information, see error code property</exception>
        ///<exception cref="ObjectDisposedException">The class instance is disposed. Any calls are prohibited.</exception>
        ///<exception cref="InvalidOperationException">The device is not opened.</exception>
        property bool ReceiveLongImage
        {
            bool get()
            {
                CheckDispose();
                if( m_hDevice == NULL )
                {
                    throw gcnew InvalidOperationException();
                }

                m_LastErrorCode = 0;
                FTR_DWORD dwFlags = 0;
                if( !ftrScanGetOptions( m_hDevice, &dwFlags ) )
                {
                    m_LastErrorCode = (int)GetLastError();
                    throw gcnew ScanAPIException( m_LastErrorCode );
                }

                return ((dwFlags & FTR_OPTIONS_RECEIVE_LONG_IMAGE) != 0);
            }

            void set( bool value )
            {
                CheckDispose();
                if( m_hDevice == NULL )
                {
                    throw gcnew InvalidOperationException();
                }
                m_LastErrorCode = 0;
                if( !ftrScanSetOptions( m_hDevice, FTR_OPTIONS_RECEIVE_LONG_IMAGE, value ? FTR_OPTIONS_RECEIVE_LONG_IMAGE : 0 ) )
                {
                    m_LastErrorCode = (int)GetLastError();
                    throw gcnew ScanAPIException( m_LastErrorCode );
                }
            }
        }

        ///<summary>
        /// Gets fingerprint present indication
        ///</summary>
        ///<exception cref="ObjectDisposedException">The class instance is disposed. Any calls are prohibited.</exception>
        ///<exception cref="InvalidOperationException">The device is not opened.</exception>
        property bool IsFingerPresent
        {
            bool get()
            {
                CheckDispose();
                if( m_hDevice == NULL )
                {
                    throw gcnew InvalidOperationException();
                }

                FTR_DWORD dwFlags = 0;
                m_LastErrorCode = 0;
                BOOL bResult = ftrScanIsFingerPresent( m_hDevice, NULL );
                if( !bResult )
                {
                    m_LastErrorCode = (int)GetLastError();
                }
                return (bResult != 0);
            }
        }

        ///<summary>
        /// Gets the device specific information
        ///</summary>
        ///<exception cref="ScanAPIException">The operation fails. To get extended error information, see error code property</exception>
        ///<exception cref="ObjectDisposedException">The class instance is disposed. Any calls are prohibited.</exception>
        ///<exception cref="InvalidOperationException">The device is not opened.</exception>
        property DeviceInfo Information
        {
            DeviceInfo get()
            {
                CheckDispose();
                if( m_hDevice == NULL )
                {
                    throw gcnew InvalidOperationException();
                }

                m_LastErrorCode = 0;
                FTRSCAN_DEVICE_INFO n_DeviceInfo;
                memset( &n_DeviceInfo, 0, sizeof( FTRSCAN_DEVICE_INFO ) );
                n_DeviceInfo.dwStructSize = sizeof( FTRSCAN_DEVICE_INFO );
                if( !ftrScanGetDeviceInfo( m_hDevice, &n_DeviceInfo ) )
                {
                    m_LastErrorCode = (int)GetLastError();
                    throw gcnew ScanAPIException( m_LastErrorCode );
                }
                DeviceInfo DevInfo;
                DevInfo.DeviceCompatibility = n_DeviceInfo.byDeviceCompatibility;
                DevInfo.imageSize.Height = n_DeviceInfo.wPixelSizeX;
                DevInfo.imageSize.Width = n_DeviceInfo.wPixelSizeY;

                return DevInfo;
            }
        }

        ///<summary>
        /// Gets the image size
        ///</summary>
        ///<exception cref="ScanAPIException">The operation fails. To get extended error information, see error code property</exception>
        ///<exception cref="ObjectDisposedException">The class instance is disposed. Any calls are prohibited.</exception>
        ///<exception cref="InvalidOperationException">The device is not opened.</exception>
        property Size ImageSize
        {
            Size get()
            {
                CheckDispose();
                if( m_hDevice == NULL )
                {
                    throw gcnew InvalidOperationException();
                }

                m_LastErrorCode = 0;
                FTRSCAN_IMAGE_SIZE n_ImageSize;
                if( !ftrScanGetImageSize( m_hDevice, &n_ImageSize ) )
                {
                    m_LastErrorCode = (int)GetLastError();
                    throw gcnew ScanAPIException( m_LastErrorCode );
                }
                Size imageSize;
                imageSize.Height = n_ImageSize.nHeight;
                imageSize.Width = n_ImageSize.nWidth;

                return imageSize;
            }
        }

        ///<summary>
        /// Gets status to green gimmick diodes.
        ///</summary>
        ///<exception cref="ScanAPIException">The operation fails. To get extended error information, see error code property</exception>
        ///<exception cref="ObjectDisposedException">The class instance is disposed. Any calls are prohibited.</exception>
        ///<exception cref="InvalidOperationException">The device is not opened.</exception>
        property bool GreenDiodesStatus
        {
            bool get()
            {
                CheckDispose();
                if( m_hDevice == NULL )
                {
                    throw gcnew InvalidOperationException();
                }

                m_LastErrorCode = 0;
                BOOL bIsGreenDiodeOn, bIsRedDiodeOn;
                if( !ftrScanGetDiodesStatus( m_hDevice, &bIsGreenDiodeOn, &bIsRedDiodeOn ) )
                {
                    m_LastErrorCode = (int)GetLastError();
                    throw gcnew ScanAPIException( m_LastErrorCode );
                }

                return (bIsGreenDiodeOn != 0);
            }
        }

        ///<summary>
        /// Gets status to red gimmick diodes.
        ///</summary>
        ///<exception cref="ScanAPIException">The operation fails. To get extended error information, see error code property</exception>
        ///<exception cref="ObjectDisposedException">The class instance is disposed. Any calls are prohibited.</exception>
        ///<exception cref="InvalidOperationException">The device is not opened.</exception>
        property bool RedDiodesStatus
        {
            bool get()
            {
                CheckDispose();
                if( m_hDevice == NULL )
                {
                    throw gcnew InvalidOperationException();
                }

                m_LastErrorCode = 0;
                BOOL bIsGreenDiodeOn, bIsRedDiodeOn;
                if( !ftrScanGetDiodesStatus( m_hDevice, &bIsGreenDiodeOn, &bIsRedDiodeOn ) )
                {
                    m_LastErrorCode = (int)GetLastError();
                    throw gcnew ScanAPIException( m_LastErrorCode );
                }

                return (bIsRedDiodeOn != 0);
            }
        }

        ///<summary>
        /// Gets last error code
        ///</summary>
        property int LastErrorCode
        {
            int get()
            {
                CheckDispose();
                if( m_hDevice == NULL )
                {
                    throw gcnew InvalidOperationException();
                }

                return m_LastErrorCode;
            }
        }

        ///<summary>
        /// Sets new status to green and red gimmick diodes.
        ///</summary>
        ///<param name='green'>New status for the green gimmick diode.</param>
        ///<param name='red'>New status for the red gimmick diode.</param>
        ///<exception cref="ScanAPIException">The operation fails. To get extended error information, see error code property</exception>
        ///<exception cref="ObjectDisposedException">The class instance is disposed. Any calls are prohibited.</exception>
        ///<exception cref="InvalidOperationException">The device is not opened.</exception>
        void SetDiodesStatus( DiodesStatus green, DiodesStatus red )
        {
            CheckDispose();
            if( m_hDevice == NULL )
            {
                throw gcnew InvalidOperationException();
            }
            m_LastErrorCode = 0;
            if( !ftrScanSetDiodesStatus( m_hDevice, (FTR_BYTE)green, (FTR_BYTE)red ) )
            {
                m_LastErrorCode = (int)GetLastError();
                throw gcnew ScanAPIException( m_LastErrorCode );
            }
        }

        ///<summary>
        /// Gets the EEPROM size in bytes.
        ///</summary>
        ///<exception cref="ScanAPIException">The operation fails. To get extended error information, see error code property</exception>
        ///<exception cref="ObjectDisposedException">The class instance is disposed. Any calls are prohibited.</exception>
        ///<exception cref="InvalidOperationException">The device is not opened.</exception>
        property int MemorySize
        {
            int get()
            {
                CheckDispose();
                if( m_hDevice == NULL )
                {
                    throw gcnew InvalidOperationException();
                }
                m_LastErrorCode = 0;
                int nSize;
                if( !ftrScanGetExtMemorySize( m_hDevice, &nSize ) )
                {
                    m_LastErrorCode = (int)GetLastError();
                    throw gcnew ScanAPIException( m_LastErrorCode );
                }

                return nSize;
            }
        }

        ///<summary>
        /// Gets device serial number
        ///</summary>
        ///<exception cref="ScanAPIException">The operation fails. To get extended error information, see error code property</exception>
        ///<exception cref="ObjectDisposedException">The class instance is disposed. Any calls are prohibited.</exception>
        ///<exception cref="InvalidOperationException">The device is not opened.</exception>
        property array<Byte>^ SerialNumber
        {
            array<Byte>^ get()
            {
                CheckDispose();
                if( m_hDevice == NULL )
                {
                    throw gcnew InvalidOperationException();
                }

                m_LastErrorCode = 0;
                array< Byte >^ sn = gcnew array< Byte >(8);
                pin_ptr< Byte > p = &sn[0];
                if( !ftrScanGetSerialNumber( m_hDevice, (PVOID)p ) )
                {
                    m_LastErrorCode = (int)GetLastError();
                    throw gcnew ScanAPIException( m_LastErrorCode );
                }

                return sn;
            }
        }

        ///<summary>
        /// Gets a raw image from the device
        ///</summary>
        ///<param name='dose'>Durability of infrared led. The value must be in the 1 ?7 range.</param>
        ///<exception cref="ScanAPIException">The operation fails. To get extended error information, see error code property</exception>
        ///<exception cref="ObjectDisposedException">The class instance is disposed. Any calls are prohibited.</exception>
        ///<exception cref="InvalidOperationException">The device is not opened.</exception>
        ///<exception cref="ArgumentOutOfRangeException">The <c>dose</c> parameter is out of range.</exception>
        array<Byte>^ GetImage( int dose )
        {
            CheckDispose();
            if( m_hDevice == NULL )
            {
                throw gcnew InvalidOperationException();
            }
            if( dose > 7 || dose < 1 )
            {
                throw gcnew ArgumentOutOfRangeException( "dose" );
            }

            m_LastErrorCode = 0;
            FTRSCAN_IMAGE_SIZE n_ImageSize;
            if( !ftrScanGetImageSize( m_hDevice, &n_ImageSize ) )
            {
                m_LastErrorCode = (int)GetLastError();
                throw gcnew ScanAPIException( m_LastErrorCode );
            }

            array< Byte >^ image = gcnew array< Byte >(n_ImageSize.nImageSize);
            pin_ptr< Byte > p = &image[0];
            if( !ftrScanGetImage2( m_hDevice, dose, (PVOID)p ) )
            {
                m_LastErrorCode = (int)GetLastError();
                throw gcnew ScanAPIException( m_LastErrorCode );
            }

            return image;
        }

        ///<summary>
        /// Gets a raw image from the device without any internal illumination.
        ///</summary>
        ///<param name='dose'>Durability of infrared led. The value must be in the 1 ?7 range.</param>
        ///<exception cref="ScanAPIException">The operation fails. To get extended error information, see error code property</exception>
        ///<exception cref="ObjectDisposedException">The class instance is disposed. Any calls are prohibited.</exception>
        ///<exception cref="InvalidOperationException">The device is not opened.</exception>
        array<Byte>^ GetDarkImage()
        {
            CheckDispose();
            if( m_hDevice == NULL )
            {
                throw gcnew InvalidOperationException();
            }

            m_LastErrorCode = 0;
            FTRSCAN_IMAGE_SIZE n_ImageSize;
            if( !ftrScanGetImageSize( m_hDevice, &n_ImageSize ) )
            {
                m_LastErrorCode = (int)GetLastError();
                throw gcnew ScanAPIException( m_LastErrorCode );
            }

            array< Byte >^ image = gcnew array< Byte >(n_ImageSize.nImageSize);
            pin_ptr< Byte > p = &image[0];
            if( !ftrScanGetDarkImage( m_hDevice, (PVOID)p ) )
            {
                m_LastErrorCode = (int)GetLastError();
                throw gcnew ScanAPIException( m_LastErrorCode );
            }

            return image;
        }

        ///<summary>
        /// Gets a frame from the device.
        ///</summary>
        ///<param name='dose'>Durability of infrared led. The value must be in the 1 ?7 range.</param>
        ///<exception cref="ScanAPIException">The operation fails. To get extended error information, see error code property</exception>
        ///<exception cref="ObjectDisposedException">The class instance is disposed. Any calls are prohibited.</exception>
        ///<exception cref="InvalidOperationException">The device is not opened.</exception>
        array<Byte>^ GetFrame()
        {
            CheckDispose();
            if( m_hDevice == NULL )
            {
                throw gcnew InvalidOperationException();
            }

            m_LastErrorCode = 0;
            FTRSCAN_IMAGE_SIZE n_ImageSize;
            if( !ftrScanGetImageSize( m_hDevice, &n_ImageSize ) )
            {
                m_LastErrorCode = (int)GetLastError();
                throw gcnew ScanAPIException( m_LastErrorCode );
            }

            array< Byte >^ image = gcnew array< Byte >(n_ImageSize.nImageSize);
            pin_ptr< Byte > p = &image[0];
            if( !ftrScanGetFrame( m_hDevice, (PVOID)p, NULL ) )
            {
                m_LastErrorCode = (int)GetLastError();
                throw gcnew ScanAPIException( m_LastErrorCode );
            }

            return image;
        }

        ///<summary>
        /// Stores a 7-bytes length buffer on the device.
        ///</summary>
        ///<param name='buffer'>the 7-bytes length buffer.</param>
        ///<exception cref="ScanAPIException">The operation fails. To get extended error information, see error code property</exception>
        ///<exception cref="ObjectDisposedException">The class instance is disposed. Any calls are prohibited.</exception>
        ///<exception cref="InvalidOperationException">The device is not opened.</exception>
        ///<exception cref="ArgumentNullException">The <c>buffer</c> parameter is a null (Nothing in VB).</exception>
        ///<exception cref="ArgumentException">The length of <c>buffer</c> parameter is out of range.</exception>
        void SaveBytes( array< Byte >^ buffer )
        {
            CheckDispose();
            if( m_hDevice == NULL )
            {
                throw gcnew InvalidOperationException();
            }
            if( buffer == nullptr )
            {
                throw gcnew ArgumentNullException( "buffer" );
            }
            if( buffer->Length != 7 )
            {
                throw gcnew ArgumentException( "buffer" );
            }

            m_LastErrorCode = 0;
            pin_ptr< Byte > p = &buffer[0];
            if( !ftrScanSave7Bytes( m_hDevice, (PVOID)p ) )
            {
                m_LastErrorCode = (int)GetLastError();
                throw gcnew ScanAPIException( m_LastErrorCode );
            }
        }

        ///<summary>
        /// Restores a 7-bytes length buffer on the device.
        ///</summary>
        ///<exception cref="ScanAPIException">The operation fails. To get extended error information, see error code property</exception>
        ///<exception cref="ObjectDisposedException">The class instance is disposed. Any calls are prohibited.</exception>
        ///<exception cref="InvalidOperationException">The device is not opened.</exception>
        array< Byte >^ RestoreBytes()
        {
            CheckDispose();
            if( m_hDevice == NULL )
            {
                throw gcnew InvalidOperationException();
            }
            m_LastErrorCode = 0;
            array< Byte >^ buffer = gcnew array< Byte >(7);
            pin_ptr< Byte > p = &buffer[0];
            if( !ftrScanRestore7Bytes( m_hDevice, (PVOID)p ) )
            {
                m_LastErrorCode = (int)GetLastError();
                throw gcnew ScanAPIException( m_LastErrorCode );
            }

            return buffer;
        }

        ///<summary>
        /// Stores the authorization code to use with SaveSecretBytes/RestoreSecretBytes functions.
        ///</summary>
        ///<param name='buffer'>the 7-bytes length authorization code.</param>
        ///<exception cref="ScanAPIException">The operation fails. To get extended error information, see error code property</exception>
        ///<exception cref="ObjectDisposedException">The class instance is disposed. Any calls are prohibited.</exception>
        ///<exception cref="InvalidOperationException">The device is not opened.</exception>
        ///<exception cref="ArgumentNullException">The <c>buffer</c> parameter is a null (Nothing in VB).</exception>
        ///<exception cref="ArgumentException">The length of <c>buffer</c> parameter is out of range.</exception>
        void SetAuthorizationCode( array< Byte >^ buffer )
        {
            CheckDispose();
            if( m_hDevice == NULL )
            {
                throw gcnew InvalidOperationException();
            }
            if( buffer == nullptr )
            {
                throw gcnew ArgumentNullException( "buffer" );
            }
            if( buffer->Length != 7 )
            {
                throw gcnew ArgumentException( "buffer" );
            }

            m_LastErrorCode = 0;
            pin_ptr< Byte > p = &buffer[0];
            if( !ftrScanSetNewAuthorizationCode( m_hDevice, (PVOID)p ) )
            {
                m_LastErrorCode = (int)GetLastError();
                throw gcnew ScanAPIException( m_LastErrorCode );
            }
        }

        ///<summary>
        /// stores a 7-bytes length buffer on the device.
        ///</summary>
        ///<param name='code'>the 7-bytes length authorization code.</param>
        ///<param name='buffer'>the 7-bytes length buffer.</param>
        ///<exception cref="ScanAPIException">The operation fails. To get extended error information, see error code property</exception>
        ///<exception cref="ObjectDisposedException">The class instance is disposed. Any calls are prohibited.</exception>
        ///<exception cref="InvalidOperationException">The device is not opened.</exception>
        ///<exception cref="ArgumentNullException">The <c>code</c> or <c>buffer</c> parameter is a null (Nothing in VB).</exception>
        ///<exception cref="ArgumentException">The length of <c>code</c> or <c>buffer</c> parameter is out of range.</exception>
        void SaveSecretBytes( array< Byte >^ code, array< Byte >^ buffer )
        {
            CheckDispose();
            if( m_hDevice == NULL )
            {
                throw gcnew InvalidOperationException();
            }
            if( code == nullptr )
            {
                throw gcnew ArgumentNullException( "buffer" );
            }
            if( code->Length != 7 )
            {
                throw gcnew ArgumentException( "buffer" );
            }
            if( buffer == nullptr )
            {
                throw gcnew ArgumentNullException( "buffer" );
            }
            if( buffer->Length != 7 )
            {
                throw gcnew ArgumentException( "buffer" );
            }

            m_LastErrorCode = 0;
            pin_ptr< Byte > c = &code[0];
            pin_ptr< Byte > p = &buffer[0];
            if( !ftrScanSaveSecret7Bytes( m_hDevice, (FTR_PVOID)c, (FTR_PVOID)p ) )
            {
                m_LastErrorCode = (int)GetLastError();
                throw gcnew ScanAPIException( m_LastErrorCode );
            }
        }

        ///<summary>
        /// restores a 7-bytes length buffer from the device.
        ///</summary>
        ///<param name='code'>the 7-bytes length authorization code.</param>
        ///<exception cref="ScanAPIException">The operation fails. To get extended error information, see error code property</exception>
        ///<exception cref="ObjectDisposedException">The class instance is disposed. Any calls are prohibited.</exception>
        ///<exception cref="InvalidOperationException">The device is not opened.</exception>
        ///<exception cref="ArgumentNullException">The <c>code</c> parameter is a null (Nothing in VB).</exception>
        ///<exception cref="ArgumentException">The length of <c>code</c> parameter is out of range.</exception>
        array< Byte >^ RestoreSecretBytes( array< Byte >^ code )
        {
            CheckDispose();
            if( m_hDevice == NULL )
            {
                throw gcnew InvalidOperationException();
            }
            if( code == nullptr )
            {
                throw gcnew ArgumentNullException( "buffer" );
            }
            if( code->Length != 7 )
            {
                throw gcnew ArgumentException( "buffer" );
            }

            m_LastErrorCode = 0;
            array< Byte >^ buffer = gcnew array< Byte >(7);
            pin_ptr< Byte > p = &buffer[0];
            pin_ptr< Byte > c = &code[0];
            if( !ftrScanRestoreSecret7Bytes( m_hDevice, (FTR_PVOID)c, (FTR_PVOID)p ) )
            {
                m_LastErrorCode = (int)GetLastError();
                throw gcnew ScanAPIException( m_LastErrorCode );
            }

            return buffer;
        }

        ///<summary>
        /// stores data to EEPROM at the specified position.
        ///</summary>
        ///<param name='data'>buffer containing the data to be written to EEPROM.</param>
        ///<param name='offset'>Specifies the number of bytes to offset the EEPROM pointer.</param>
        ///<param name='count'>Number of bytes to be written to EEPROM.</param>
        ///<exception cref="ScanAPIException">The operation fails. To get extended error information, see error code property</exception>
        ///<exception cref="ObjectDisposedException">The class instance is disposed. Any calls are prohibited.</exception>
        ///<exception cref="InvalidOperationException">The device is not opened.</exception>
        ///<exception cref="ArgumentNullException">The <c>data</c> parameter is a null (Nothing in VB).</exception>
        ///<exception cref="ArgumentException">The <c>offset</c> or <c>count</c> parameter is less than 0.</exception>
        ///<exception cref="ArgumentOutOfRangeException">The <c>count</c> is more than length of buffer data or the 
        /// sum of <c>offset</c> and <c>count</c> is more than memory size.</exception>
        void SaveMemory( array< Byte >^ data, int offset, int count )
        {
            CheckDispose();
            if( m_hDevice == NULL )
            {
                throw gcnew InvalidOperationException();
            }
            if( data == nullptr )
            {
                throw gcnew ArgumentNullException( "buffer" );
            }
            if( data->Length < count )
            {
                throw gcnew ArgumentOutOfRangeException( "count" );
            }

            if( offset < 0 )
            {
                throw gcnew ArgumentException( "offset" );
            }
            if( count < 0 )
            {
                throw gcnew ArgumentException( "count" );
            }

            int nMemSize = this->MemorySize;
            if( offset + count > nMemSize )
            {
                throw gcnew ArgumentOutOfRangeException();
            }
            m_LastErrorCode = 0;
            pin_ptr< Byte > p = &data[0];
            if( !ftrScanSaveExtMemory( m_hDevice, (FTR_PVOID)p, offset, count ) )
            {
                m_LastErrorCode = (int)GetLastError();
                throw gcnew ScanAPIException( m_LastErrorCode );
            }
        }

        ///<summary>
        /// restores data from EEPROM at the specified position.
        ///</summary>
        ///<param name='offset'>Specifies the number of bytes to offset the EEPROM pointer.</param>
        ///<param name='count'>Number of bytes to be read from EEPROM</param>
        ///<returns>the data readed from EEPROM</returns>
        ///<exception cref="ScanAPIException">The operation fails. To get extended error information, see error code property</exception>
        ///<exception cref="ObjectDisposedException">The class instance is disposed. Any calls are prohibited.</exception>
        ///<exception cref="InvalidOperationException">The device is not opened.</exception>
        ///<exception cref="ArgumentNullException">The <c>data</c> parameter is a null (Nothing in VB).</exception>
        ///<exception cref="ArgumentException">The <c>offset</c> or <c>count</c> parameter is less than 0.</exception>
        ///<exception cref="ArgumentOutOfRangeException">The <c>count</c> is more than length of buffer data or the 
        /// sum of <c>offset</c> and <c>count</c> is more than memory size.</exception>
        array< Byte >^ RestoreMemory( int offset, int count )
        {
            CheckDispose();
            if( m_hDevice == NULL )
            {
                throw gcnew InvalidOperationException();
            }
            if( offset < 0 )
            {
                throw gcnew ArgumentException( "offset" );
            }
            if( count < 0 )
            {
                throw gcnew ArgumentException( "count" );
            }
            int nMemSize = this->MemorySize;
            if( offset + count > nMemSize )
            {
                throw gcnew ArgumentOutOfRangeException();
            }

            m_LastErrorCode = 0;
            array< Byte >^ data = gcnew array< Byte >(count);
            pin_ptr< Byte > p = &data[0];
            if( !ftrScanRestoreExtMemory( m_hDevice, (FTR_PVOID)p, offset, count ) )
            {
                m_LastErrorCode = (int)GetLastError();
                throw gcnew ScanAPIException( m_LastErrorCode );
            }

            return data;
        }

    public:
        ///<summary>
        /// Gets the device status for each interface.
        ///</summary>
        ///<returns>the array of device status</returns>
        ///<exception cref="ScanAPIException">The operation fails. To get extended error information, see error code property</exception>
        static array<FTRSCAN_INTERFACE_STATUS>^ GetInterfaces()
        {
            FTRSCAN_INTERFACES_LIST list;
            if( !ftrScanGetInterfaces( &list ) )
            {
                throw gcnew ScanAPIException( (int)GetLastError() );
            }
            array<FTRSCAN_INTERFACE_STATUS>^ statusArray = gcnew array<FTRSCAN_INTERFACE_STATUS>( FTR_MAX_INTERFACE_NUMBER );
            for( int index = 0; index < FTR_MAX_INTERFACE_NUMBER; index++ )
            {
                statusArray[ index ] = (FTRSCAN_INTERFACE_STATUS)list.InterfaceStatus[ index ];
            }
            return statusArray;
        }


        ///<summary>
        /// Gets\Sets the default interface number.
        ///</summary>
        ///<exception cref="ArgumentOutOfRangeException">The <c>value</c> parameter is out of range.
        /// The value must be between 0 and 127</exception>
        static property int BaseInterface
        {
            int get()
            {
                return ftrGetBaseInterfaceNumber();
            }

            void set( int value )
            {
                if( value >= FTR_MAX_INTERFACE_NUMBER || value < 0 )
                {
                    throw gcnew ArgumentOutOfRangeException( "value" );
                }
                if( !ftrSetBaseInterface( value ) )
                {
                    throw gcnew ScanAPIException( (int)GetLastError() );
                }
            }
        }

        ///<summary>
        /// Changes the logging facility level.
        ///</summary>
        ///<param name='mask'>Specify where logging output should be directed.</param>
        ///<param name='level'>Specifies the log level.</param>
        ///<param name='fileName'>Specifies the name of the log file.</param>
        ///<returns><c>true</c> if the function succeeds otherwise <c>false</c></returns>
        static bool SetLoggingFacilityLevel( LogMask mask, LogLevel level, String^ fileName )
        {
            IntPtr pointer = System::Runtime::InteropServices::Marshal::StringToHGlobalAnsi( fileName );
            FTR_BOOL bResult = ftrSetLoggingFacilityLevel( (FTR_DWORD)mask, (FTR_DWORD)level, (char*)pointer.ToPointer() );
            System::Runtime::InteropServices::Marshal::FreeHGlobal( pointer );

            return bResult ? true : false;
        }

	public:
		///<summary>
		///ftrWSQ functions
		///</summary>
		array<Byte>^ WSQ_FromRawImage(array<Byte>^ rawImage, int nWidth, int nHeight, float fBitrate)
		{
            CheckDispose();
            if( m_hDevice == NULL )
            {
                throw gcnew InvalidOperationException();
            }
			FTRIMGPARMS imgParm;
            array< Byte >^ wsqTemp = gcnew array< Byte >(nWidth*nHeight);
            pin_ptr< Byte > pWsq = &wsqTemp[0];
            pin_ptr< Byte > pRaw = &rawImage[0];
			imgParm.Bitrate = fBitrate;	
			imgParm.DPI = -1;
			imgParm.Width = nWidth;
			imgParm.Height = nHeight;
			imgParm.RAW_size =  nWidth * nHeight;

			if( !ftrWSQ_FromRAWImage(m_hDevice, pRaw, &imgParm, pWsq) )
			{
				m_LastErrorCode = (int)GetLastError();
                throw gcnew ScanAPIException( m_LastErrorCode );
			}
			array< Byte >^ wsqImage = gcnew array< Byte >(imgParm.WSQ_size);
			for( FTR_DWORD i=0; i<imgParm.WSQ_size; i++ )
				wsqImage[i] = wsqTemp[i];
			return wsqImage;
		}

		WsqImageParameter WSQ_GetImageParameters( array<Byte>^ wsqImage )
		{
			FTRIMGPARMS imgParm;
			imgParm.WSQ_size = wsqImage->Length;
			pin_ptr< Byte > pWsq = &wsqImage[0];
			if( !ftrWSQ_GetImageParameters(pWsq, &imgParm) )
			{
				m_LastErrorCode = (int)GetLastError();
                throw gcnew ScanAPIException( m_LastErrorCode );
			}
			WsqImageParameter wsqParm;
			wsqParm.Width = imgParm.Width;
			wsqParm.Height = imgParm.Height;   
			wsqParm.DPI = imgParm.DPI;         
			wsqParm.RAW_size = imgParm.RAW_size;   
			wsqParm.BMP_size = imgParm.BMP_size;    
			wsqParm.WSQ_size = imgParm.WSQ_size;   
			wsqParm.Bitrate =imgParm.Bitrate;	 
			return wsqParm;
		}

		array<Byte>^ WSQ_ToRAWImage( array<Byte>^ wsqImage, int rawSize )
		{
			FTRIMGPARMS imgParm;
            array< Byte >^ rawImage = gcnew array< Byte >(rawSize);
            pin_ptr< Byte > pWsq = &wsqImage[0];
            pin_ptr< Byte > pRaw = &rawImage[0];	
			if( !ftrWSQ_ToRawImage(pWsq, &imgParm, pRaw) )
			{
				m_LastErrorCode = (int)GetLastError();
                throw gcnew ScanAPIException( m_LastErrorCode );
			}
			return rawImage;
		}

    protected:
        ///<summary>
        /// The Finalize method.
        ///</summary>
        !Device()
        {
            if( m_bDispose )
                return;

            if( m_hDevice != NULL )
            {
                ftrScanCloseDevice( m_hDevice );
                m_hDevice = NULL;
            }
        }

        ///<summary>
		/// If the class is disposed, this function raises an exception.
        ///</summary>
        ///<remarks>
		/// This function must be called before any operation in all functions.
        ///</remarks>
        void CheckDispose()
        {
            if( m_bDispose )
            {
                throw gcnew ObjectDisposedException( this->GetType()->FullName );
            }
        }

        ///<summary>
        /// If the class is deleted by calling <c>Dispose</c>, m_bDispose is true.
        ///</summary>
        ///<remarks>
        /// After of calling <c>Dispose</c>, the class cannot be used. 
        /// The class raises the <c>ObjectDisposedException</c> exception in
		/// the event of an invalid usage condition. 
        ///</remarks>
        bool m_bDispose;

        ///<summary>
        /// Last error code.
        ///</summary>
        int m_LastErrorCode;

        FTRHANDLE   m_hDevice;
    };
}
