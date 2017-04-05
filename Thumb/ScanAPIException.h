#pragma once
using namespace System;

namespace ScanAPIHelper
{
    public ref class ScanAPIException : public Exception
    {
    public:
        ScanAPIException( int errorCode )
            : Exception()
            , m_ErrorCode( errorCode )
        {
        }

        ScanAPIException( int errorCode, String ^ message )
            : Exception( message )
            , m_ErrorCode( errorCode )
        {
        }

        ///<summary>
        /// Gets error code
        ///</summary>
        property int ErrorCode
        {
            int get()
            {
                return m_ErrorCode;
            }
        }
    protected:

        int m_ErrorCode;
    };
}
