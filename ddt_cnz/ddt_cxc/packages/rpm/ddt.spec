#
# spec file for configuration of package apache
#
# Copyright  (c)  2015  Ericsson LM
# This file and all modifications and additions to the pristine
# package are under the same license as the package itself.
#
# please send bugfixes or comments to paolo.palmieri@ericsson.com
#

%define packer %(finger -lp `echo "$USER"` | head -n 1 | cut -d: -f 3)
%define _topdir /var/tmp/%(echo "$USER")/rpms/
%define _tmppath %_topdir/tmp
%define ddt_cxc_path %{_cxcdir}
%define model_path /opt/com/etc/model

Requires: APOS_OSCONFBIN

Name:      %{_name} 
Summary:   Installation package for AES DDT. 
Version:   %{_prNr} 
Release:   %{_rel} 
License:   Ericsson Proprietary 
Vendor:    Ericsson LM 
Packager:  %packer 
Group:     Library 
BuildRoot: %_tmppath 

%description
Installation package for AES DDT

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
if [ ! -d $RPM_BUILD_ROOT%model_path ]
then
        mkdir -p $RPM_BUILD_ROOT%model_path
fi

cp %ddt_cxc_path/bin/aes_ddtd $RPM_BUILD_ROOT%AESBINdir/aes_ddtd

cp %ddt_cxc_path/conf/c_AxeDirectDataTransfer_imm_classes.xml $RPM_BUILD_ROOT%AESCONFdir/c_AxeDirectDataTransfer_imm_classes.xml
cp %ddt_cxc_path/conf/o_AxeDirectDataTransferdirectDataTransfer_imm_objects.xml $RPM_BUILD_ROOT%AESCONFdir/o_AxeDirectDataTransferdirectDataTransfer_imm_objects.xml
cp %ddt_cxc_path/conf/AxeDirectDataTransfer_mp.xml $RPM_BUILD_ROOT%model_path/AxeDirectDataTransfer_mp.xml

cp %ddt_cxc_path/bin/aes_ddt_2N_clc $RPM_BUILD_ROOT%AESBINdir/aes_ddt_2N_clc

%post
if [ $1 == 1 ]
then
        echo "This is the %{_name} package %{_rel} post-install script during installation phase"
fi
if [ $1 == 2 ]
then
        echo "This is the %{_name} package %{_rel} post-install script during upgrade phase"
fi

chmod +x $RPM_BUILD_ROOT%AESBINdir/aes_ddtd
chmod +x $RPM_BUILD_ROOT%AESBINdir/aes_ddt_2N_clc
chmod +x $RPM_BUILD_ROOT%AESCONFdir/c_AxeDirectDataTransfer_imm_classes.xml
chmod +x $RPM_BUILD_ROOT%AESCONFdir/o_AxeDirectDataTransferdirectDataTransfer_imm_objects.xml
chmod +x $RPM_BUILD_ROOT%model_path/AxeDirectDataTransfer_mp.xml

ln -sf $RPM_BUILD_ROOT%AESBINdir/aes_ddtd $RPM_BUILD_ROOT%{_bindir}/aes_ddtd
ln -sf $RPM_BUILD_ROOT%AESBINdir/aes_ddt_2N_clc $RPM_BUILD_ROOT%{_bindir}/aes_ddt_2N_clc

%preun
if [ $1 == 0 ]
then
        echo "This is the %{_name} package %{_rel} pre-uninstall script during uninstall phase"

	rm -f %{_bindir}/aes_ddtd
	rm -f %{_bindir}/aes_ddt_2N_clc
fi
if [ $1 == 1 ]
then
        echo "This is the %{_name} package %{_rel} pre-uninstall script during upgrade phase"
fi

%postun
if [ $1 == 0 ]
then
        echo "This is the %{_name} package %{_rel} post-uninstall script during uninstall phase"

	rm -f %AESBINdir/aes_ddtd
	rm -f %AESBINdir/aes_ddt_2N_clc
	rm -f %AESCONFdir/c_AxeDirectDataTransfer_imm_classes.xml
	rm -f %AESCONFdir/o_AxeDirectDataTransferdirectDataTransfer_imm_objects.xml
	rm -f %model_path/AxeDirectDataTransfer_mp.xml
fi

if [ $1 == 1 ]
then
        echo "This is the %{_name} package %{_rel} post-uninstall script during upgrade phase"
fi

%files
%defattr(-,root,root)
%AESBINdir/aes_ddtd
%AESBINdir/aes_ddt_2N_clc
%AESCONFdir/c_AxeDirectDataTransfer_imm_classes.xml
%AESCONFdir/o_AxeDirectDataTransferdirectDataTransfer_imm_objects.xml
%model_path/AxeDirectDataTransfer_mp.xml

%changelog
* Thu Mar 26 2015 - paolo.elefante@ericsson.com
- First version

