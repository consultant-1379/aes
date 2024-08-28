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
%define afp_cxc_path %{_cxcdir}

Name:      %{_name} 
Summary:   Installation package for AFP. 
Version:   %{_prNr} 
Release:   %{_rel} 
License:   Ericsson Proprietary 
Vendor:    Ericsson LM 
Packager:  %packer 
Group:     Library 
BuildRoot: %_tmppath 
Requires: APOS_OSCONFBIN
%description
Installation package for AFP

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

cp %afp_cxc_path/bin/aes_afpd 		$RPM_BUILD_ROOT%AESBINdir/aes_afpd
cp %afp_cxc_path/bin/lib_ext/libaes_afp.so.1.10.2 $RPM_BUILD_ROOT%AESLIB64dir/libaes_afp.so.1.10.2
cp %afp_cxc_path/bin/afpdef 		$RPM_BUILD_ROOT%AESBINdir/afpdef
cp %afp_cxc_path/bin/afpdef.sh 		$RPM_BUILD_ROOT%AESBINdir/afpdef.sh
cp %afp_cxc_path/bin/afprm 		    $RPM_BUILD_ROOT%AESBINdir/afprm
cp %afp_cxc_path/bin/afprm.sh 		    $RPM_BUILD_ROOT%AESBINdir/afprm.sh
cp %afp_cxc_path/bin/afpls 		    $RPM_BUILD_ROOT%AESBINdir/afpls
cp %afp_cxc_path/bin/afpls.sh 		    $RPM_BUILD_ROOT%AESBINdir/afpls.sh
cp %afp_cxc_path/bin/aes_afp_clc	$RPM_BUILD_ROOT%AESBINdir/aes_afp_clc
cp %afp_cxc_path/conf/ha_aes_afp_objects.xml $RPM_BUILD_ROOT%AESCONFdir/ha_aes_afp_objects.xml

%post
if [ $1 == 1 ]
then
        echo "This is the %{_name} package %{_rel} post-install script during installation phase"
fi
if [ $1 == 2 ]
then
        echo "This is the %{_name} package %{_rel} post-install script during upgrade phase"
fi

chmod +x $RPM_BUILD_ROOT%AESBINdir/aes_afpd
chmod +x $RPM_BUILD_ROOT%AESBINdir/afpdef
chmod +x $RPM_BUILD_ROOT%AESBINdir/afpdef.sh
chmod +x $RPM_BUILD_ROOT%AESBINdir/afprm
chmod +x $RPM_BUILD_ROOT%AESBINdir/afprm.sh
chmod +x $RPM_BUILD_ROOT%AESBINdir/afpls
chmod +x $RPM_BUILD_ROOT%AESBINdir/afpls.sh
chmod +x $RPM_BUILD_ROOT%AESBINdir/aes_afp_clc
chmod +x $RPM_BUILD_ROOT%AESLIB64dir/libaes_afp.so.1.10.2
chmod +x $RPM_BUILD_ROOT%AESCONFdir/ha_aes_afp_objects.xml
ln -sf $RPM_BUILD_ROOT%AESLIB64dir/libaes_afp.so.1.10.2 $RPM_BUILD_ROOT%{_lib64dir}/libaes_afp.so.1
ln -sf $RPM_BUILD_ROOT%{_lib64dir}/libaes_afp.so.1 $RPM_BUILD_ROOT%{_lib64dir}/libaes_afp.so
ln -sf $RPM_BUILD_ROOT%AESBINdir/aes_afpd $RPM_BUILD_ROOT%{_bindir}/aes_afpd
ln -sf $RPM_BUILD_ROOT%AESBINdir/afpdef.sh $RPM_BUILD_ROOT%{_bindir}/afpdef
ln -sf $RPM_BUILD_ROOT%AESBINdir/afprm.sh $RPM_BUILD_ROOT%{_bindir}/afprm
ln -sf $RPM_BUILD_ROOT%AESBINdir/afpls.sh $RPM_BUILD_ROOT%{_bindir}/afpls
ln -sf $RPM_BUILD_ROOT%AESBINdir/aes_afp_clc $RPM_BUILD_ROOT%{_bindir}/aes_afp_clc

%preun
if [ $1 == 0 ]
then
        echo "This is the %{_name} package %{_rel} pre-uninstall script during uninstall phase"

	rm -f %{_lib64dir}/libaes_afp.so.1
	rm -f %{_lib64dir}/libaes_afp.so
	rm -f %{_bindir}/aes_afpd
	rm -f %{_bindir}/afpdef
	rm -f %{_bindir}/afprm
	rm -f %{_bindir}/afpls
	rm -f %{_bindir}/aes_afp_clc
fi
if [ $1 == 1 ]
then
        echo "This is the %{_name} package %{_rel} pre-uninstall script during upgrade phase"
fi

%postun
if [ $1 == 0 ]
then
        echo "This is the %{_name} package %{_rel} post-uninstall script during uninstall phase"

	rm -f %AESLIB64dir/libaes_afp.so.1.10.2
	rm -f %AESBINdir/aes_afpd
	rm -f %AESBINdir/afpdef
	rm -f %AESBINdir/afpdef.sh
	rm -f %AESBINdir/afprm
	rm -f %AESBINdir/afprm.sh
	rm -f %AESBINdir/afpls
	rm -f %AESBINdir/afpls.sh
	rm -f %AESBINdir/aes_afp_clc
	rm -f %AESCONFdir/ha_aes_afp_objects.xml
fi

if [ $1 == 1 ]
then
        echo "This is the %{_name} package %{_rel} post-uninstall script during upgrade phase"
fi

%files
%defattr(-,root,root)
%AESBINdir/aes_afpd
%AESBINdir/afpdef
%AESBINdir/afpdef.sh
%AESBINdir/afprm
%AESBINdir/afprm.sh
%AESBINdir/afpls
%AESBINdir/afpls.sh
%AESBINdir/aes_afp_clc
%AESLIB64dir/libaes_afp.so.1.10.2
%AESCONFdir/ha_aes_afp_objects.xml

%changelog
* Fri Oct 10 2014 - n.naresh (at) tcs.com
* Mon Oct 17 2011 - Sarita  (at) tcs.com 

