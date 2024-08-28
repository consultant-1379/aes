#
# spec file for configuration of package apache
#
# Copyright  (c)  2010  Ericsson LM
# This file and all modifications and additions to the pristine
# package are under the same license as the package itself.
#
# please send bugfixes or comments to paolo.palmieri@ericsson.com
#

%define packer %(finger -lp `echo "$USER"` | head -n 1 | cut -d: -f 3)
%define _topdir /var/tmp/%(echo "$USER")/rpms/
%define _tmppath %_topdir/tmp
%define dbo_cxc_path %{_cxcdir}

Name:      %{_name} 
Summary:   Installation package for DBO. 
Version:   %{_prNr} 
Release:   %{_rel} 
License:   Ericsson Proprietary 
Vendor:    Ericsson LM 
Packager:  %packer 
Group:     Library 
BuildRoot: %_tmppath 
Requires:  APOS_OSCONFBIN

%description
Installation package for DBO

%pre
if [ $1 == 1 ]
then
	echo "This is the %{_name} package %{_rel} pre-install script during installation phase"
fi
if [ $1 == 2 ]
then
	echo "This is the %{_name} package %{_rel} pre-install script during upgrade phase"
fi

%install
echo "This is the %{_name} package %{_rel} install script"

if [ ! -d $RPM_BUILD_ROOT%rootdir ]
then
        mkdir $RPM_BUILD_ROOT%rootdir
fi
if [ ! -d $RPM_BUILD_ROOT%APdir ]
then
        mkdir $RPM_BUILD_ROOT%APdir
fi
if  [ ! -d $RPM_BUILD_ROOT%AESdir ]
then
        mkdir $RPM_BUILD_ROOT%AESdir
fi
if [ ! -d  $RPM_BUILD_ROOT%AESBINdir ]
then
        mkdir $RPM_BUILD_ROOT%AESBINdir
fi
if [ ! -d $RPM_BUILD_ROOT%AESCONFdir ]
then
        mkdir $RPM_BUILD_ROOT%AESCONFdir
fi
if [ ! -d $RPM_BUILD_ROOT%AESLIB64dir ]
then
        mkdir $RPM_BUILD_ROOT%AESLIB64dir
fi

cp %dbo_cxc_path/bin/aes_dbod	$RPM_BUILD_ROOT%AESBINdir/aes_dbod
cp %dbo_cxc_path/bin/aes_dbo_clc	$RPM_BUILD_ROOT%AESBINdir/aes_dbo_clc
cp %dbo_cxc_path/bin/lib_ext/libaes_dbo.so.2.2.0 $RPM_BUILD_ROOT%AESLIB64dir/libaes_dbo.so.2.2.0
cp %dbo_cxc_path/conf/ha_aes_dbo_objects.xml $RPM_BUILD_ROOT%AESCONFdir/ha_aes_dbo_objects.xml

%post
if [ $1 == 1 ]
then
        echo "This is the %{_name} package %{_rel} post-install script during installation phase"
fi
if [ $1 == 2 ]
then
        echo "This is the %{_name} package %{_rel} post-install script during upgrade phase"
fi

chmod +x $RPM_BUILD_ROOT%AESBINdir/aes_dbod
chmod +x $RPM_BUILD_ROOT%AESBINdir/aes_dbo_clc
chmod +x $RPM_BUILD_ROOT%AESLIB64dir/libaes_dbo.so.2.2.0
chmod +x $RPM_BUILD_ROOT%AESCONFdir/ha_aes_dbo_objects.xml
ln -sf $RPM_BUILD_ROOT%AESLIB64dir/libaes_dbo.so.2.2.0 $RPM_BUILD_ROOT%{_lib64dir}/libaes_dbo.so.2
ln -sf $RPM_BUILD_ROOT%{_lib64dir}/libaes_dbo.so.2 $RPM_BUILD_ROOT%{_lib64dir}/libaes_dbo.so
ln -sf $RPM_BUILD_ROOT%AESBINdir/aes_dbod $RPM_BUILD_ROOT%{_bindir}/aes_dbod
ln -sf $RPM_BUILD_ROOT%AESBINdir/aes_dbo_clc $RPM_BUILD_ROOT%{_bindir}/aes_dbo_clc

%preun
if [ $1 == 0 ]
then
        echo "This is the %{_name} package %{_rel} pre-uninstall script during uninstall phase"

	rm -f %{_lib64dir}/libaes_dbo.so.2
	rm -f %{_lib64dir}/libaes_dbo.so
	rm -f %{_bindir}/aes_dbod
	rm -f %{_bindir}/aes_dbo_clc
fi
if [ $1 == 1 ]
then
        echo "This is the %{_name} package %{_rel} pre-uninstall script during upgrade phase"
fi

%postun
if [ $1 == 0 ]
then
        echo "This is the %{_name} package %{_rel} post-uninstall script during uninstall phase"

	rm -f %AESLIB64dir/libaes_dbo.so.2.2.0
	rm -f %AESBINdir/aes_dbod
	rm -f %AESBINdir/aes_dbo_clc
	rm -f %AESCONFdir/ha_aes_dbo_objects.xml
fi

if [ $1 == 1 ]
then
        echo "This is the %{_name} package %{_rel} post-uninstall script during upgrade phase"
fi

%files
%defattr(-,root,root)
%AESBINdir/aes_dbod
%AESBINdir/aes_dbo_clc
%AESLIB64dir/libaes_dbo.so.2.2.0
%AESCONFdir/ha_aes_dbo_objects.xml

%changelog
* Mon Oct 10 2011 - tanu.aggarwal (at) tcs.com
* Wed Sep 07 2011 - chenna.madhavi (at) tcs.com

