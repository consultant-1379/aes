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
%define cdh_cxc_path %{_cxcdir}

Name:      %{_name} 
Summary:   Installation package for CDH. 
Version:   %{_prNr} 
Release:   %{_rel} 
License:   Ericsson Proprietary 
Vendor:    Ericsson LM 
Packager:  %packer 
Group:     Library 
BuildRoot: %_tmppath 
Requires: APOS_OSCONFBIN
%description
Installation package for CDH

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

cp %cdh_cxc_path/bin/aes_cdhd	$RPM_BUILD_ROOT%AESBINdir/aes_cdhd
cp %cdh_cxc_path/bin/cdhdsls	$RPM_BUILD_ROOT%AESBINdir/cdhdsls
cp %cdh_cxc_path/bin/cdhls	$RPM_BUILD_ROOT%AESBINdir/cdhls
cp %cdh_cxc_path/bin/aes_cdh_clc	$RPM_BUILD_ROOT%AESBINdir/aes_cdh_clc
cp %cdh_cxc_path/bin/cdhls.sh	$RPM_BUILD_ROOT%AESBINdir/cdhls.sh
cp %cdh_cxc_path/bin/cdhdsls.sh $RPM_BUILD_ROOT%AESBINdir/cdhdsls.sh
cp %cdh_cxc_path/bin/lib_ext/libaes_cdh.so.1.5.1 $RPM_BUILD_ROOT%AESLIB64dir/libaes_cdh.so.1.5.1
cp %cdh_cxc_path/conf/ha_aes_cdh_objects.xml $RPM_BUILD_ROOT%AESCONFdir/ha_aes_cdh_objects.xml
cp %cdh_cxc_path/conf/AxeDataTransfer_imm_classes.xml $RPM_BUILD_ROOT%AESCONFdir/AxeDataTransfer_imm_classes.xml
cp %cdh_cxc_path/conf/AxeDataTransfer_imm_objects.xml $RPM_BUILD_ROOT%AESCONFdir/AxeDataTransfer_imm_objects.xml
cp %cdh_cxc_path/conf/AxeDataTransfer_mp.xml $RPM_BUILD_ROOT%AESCONFdir/AxeDataTransfer_mp.xml

%post
if [ $1 == 1 ]
then
        echo "This is the %{_name} package %{_rel} post-install script during installation phase"
fi
if [ $1 == 2 ]
then
        echo "This is the %{_name} package %{_rel} post-install script during upgrade phase"
fi

chmod +x $RPM_BUILD_ROOT%AESBINdir/aes_cdhd
chmod +x $RPM_BUILD_ROOT%AESBINdir/cdhdsls
chmod +x $RPM_BUILD_ROOT%AESBINdir/cdhdsls.sh
chmod +x $RPM_BUILD_ROOT%AESBINdir/cdhls
chmod +x $RPM_BUILD_ROOT%AESBINdir/cdhls.sh
chmod +x $RPM_BUILD_ROOT%AESBINdir/aes_cdh_clc
chmod +x $RPM_BUILD_ROOT%AESLIB64dir/libaes_cdh.so.1.5.1
chmod +x $RPM_BUILD_ROOT%AESCONFdir/ha_aes_cdh_objects.xml
chmod +x $RPM_BUILD_ROOT%AESCONFdir/AxeDataTransfer_imm_classes.xml
chmod +x $RPM_BUILD_ROOT%AESCONFdir/AxeDataTransfer_imm_objects.xml
chmod +x $RPM_BUILD_ROOT%AESCONFdir/AxeDataTransfer_mp.xml
ln -sf $RPM_BUILD_ROOT%AESLIB64dir/libaes_cdh.so.1.5.1 $RPM_BUILD_ROOT%{_lib64dir}/libaes_cdh.so.1
ln -sf $RPM_BUILD_ROOT%{_lib64dir}/libaes_cdh.so.1 $RPM_BUILD_ROOT%{_lib64dir}/libaes_cdh.so
ln -sf $RPM_BUILD_ROOT%AESBINdir/aes_cdhd $RPM_BUILD_ROOT%{_bindir}/aes_cdhd
ln -sf $RPM_BUILD_ROOT%AESBINdir/cdhdsls.sh $RPM_BUILD_ROOT%{_bindir}/cdhdsls
ln -sf $RPM_BUILD_ROOT%AESBINdir/cdhls.sh $RPM_BUILD_ROOT%{_bindir}/cdhls
ln -sf $RPM_BUILD_ROOT%AESBINdir/aes_cdh_clc $RPM_BUILD_ROOT%{_bindir}/aes_cdh_clc

%preun
if [ $1 == 0 ]
then
        echo "This is the %{_name} package %{_rel} pre-uninstall script during uninstall phase"

	rm -f %{_lib64dir}/libaes_cdh.so.1
	rm -f %{_lib64dir}/libaes_cdh.so
	rm -f %{_bindir}/aes_cdhd
	rm -f %{_bindir}/cdhdsls
	rm -f %{_bindir}/cdhls
	rm -f %{_bindir}/aes_cdh_clc
fi
if [ $1 == 1 ]
then
        echo "This is the %{_name} package %{_rel} pre-uninstall script during upgrade phase"
fi

%postun
if [ $1 == 0 ]
then
        echo "This is the %{_name} package %{_rel} post-uninstall script during uninstall phase"

	rm -f %AESLIB64dir/libaes_cdh.so.1.5.1
	rm -f %AESBINdir/aes_cdhd
	rm -f %AESBINdir/cdhdsls
	rm -f %AESBINdir/cdhdsls.sh
	rm -f %AESBINdir/cdhls
	rm -f %AESBINdir/cdhls.sh
	rm -f %AESBINdir/aes_cdh_clc
	rm -f %AESCONFdir/ha_aes_cdh_objects.xml
	rm -f %AESCONFdir/AxeDataTransfer_imm_classes.xml
	rm -f %AESCONFdir/AxeDataTransfer_imm_objects.xml
	rm -f %AESCONFdir/AxeDataTransfer_mp.xml
fi

if [ $1 == 1 ]
then
        echo "This is the %{_name} package %{_rel} post-uninstall script during upgrade phase"
fi

%files
%defattr(-,root,root)
%AESBINdir/aes_cdhd
%AESBINdir/cdhdsls
%AESBINdir/cdhdsls.sh
%AESBINdir/cdhls
%AESBINdir/cdhls.sh
%AESBINdir/aes_cdh_clc
%AESLIB64dir/libaes_cdh.so.1.5.1
%AESCONFdir/ha_aes_cdh_objects.xml
%AESCONFdir/AxeDataTransfer_imm_classes.xml
%AESCONFdir/AxeDataTransfer_imm_objects.xml
%AESCONFdir/AxeDataTransfer_mp.xml

%changelog
* Mon Oct 10 2011 - tanu.aggarwal (at) tcs.com
* Wed Sep 07 2011 - chenna.madhavi (at) tcs.com

