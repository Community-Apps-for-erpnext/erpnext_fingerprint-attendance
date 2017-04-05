del .\Signed\ScanAPIHelper.* /F
"C:\Program Files (x86)\Microsoft SDKs\Windows\v8.1A\bin\NETFX 4.5.1 Tools\ildasm.exe" .\ScanAPIHelper.dll /out:.\Signed\ScanAPIHelper.il
"C:\Windows\Microsoft.NET\Framework64\v4.0.30319\ilasm.exe" .\Signed\ScanAPIHelper.il /dll /key=.\ScanAPIHelper.snk /output=.\Signed\ScanAPIHelper.dll

pause