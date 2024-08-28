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
%define ohi_cxc_path %{_cxcdir}

Name:      %{_name}
Summary:   Installation package for AES OHI library.
Version:   %{_prNr}
Release:   %{_rel} 
License:   Ericsson Proprietary
Vendor:    Ericsson LM
Packager:  %packer
Group:     Library
BuildRoot: %_tmppath
Requires: APOS_OSCONFBIN

%description
Installation package for OHI API.

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
if [ ! -d $RPM_BUILD_ROOT%AESLIB64dir ]
then
        mkdir $RPM_BUILD_ROOT%AESLIB64dir
fi

cp %ohi_cxc_path/bin/lib_ext/libaes_ohi.so.1.6.1  $RPM_BUILD_ROOT%AESLIB64dir/libaes_ohi.so.1.6.1

%post
if [ $1 == 1 ]
then
        echo "This is the %{_name} package %{_rel} post-install script during installation phase"
fi
if [ $1 == 2 ]
then
        echo "This is the %{_name} package %{_rel} post-install script during upgrade phase"
fi

chmod +x $RPM_BUILD_ROOT%AESLIB64dir/libaes_ohi.so.1.6.1
ln -sf $RPM_BUILD_ROOT%AESLIB64dir/libaes_ohi.so.1.6.1  $RPM_BUILD_ROOT%{_lib64dir}/libaes_ohi.so.1
ln -sf $RPM_BUILD_ROOT%{_lib64dir}/libaes_ohi.so.1  $RPM_BUILD_ROOT%{_lib64dir}/libaes_ohi.so

%preun
if [ $1 == 0 ]
then
        echo "This is the %{_name} package %{_rel} pre-uninstall script during uninstall phase"
	rm -f %{_lib64dir}/libaes_ohi.so.1
	rm -f %{_lib64dir}/libaes_ohi.so
fi
if [ $1 == 1 ]
then
        echo "This is the %{_name} package %{_rel} pre-uninstall script during upgrade phase"
fi

%postun
if [ $1 == 0 ]
then
        echo "This is the %{_name} package %{_rel} post-uninstall script during uninstall phase"
	rm -f %AESLIB64dir/libaes_ohi.so.1.6.1
fi

if [ $1 == 1 ]
then
        echo "This is the %{_name} package %{_rel} post-uninstall script during upgrade phase"
fi

%files
%defattr(-,root,root)
%AESLIB64dir/libaes_ohi.so.1.6.1

%changelog
* Fri Jul 29 2011 - chenna.madhavi (at) tcs.com
- Updated spec file according to PA5 of AP Directory Structure.
- Added macros for Name, Version and Release
- Modified spec file according to new DR

