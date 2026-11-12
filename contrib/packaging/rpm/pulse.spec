Name:           pulse
Version:        0.6.2
Release:        1%{?dist}
Summary:        Lightweight Linux process monitoring tool

%global debug_package %{nil}

License:        MIT
URL:            https://github.com/cherries-works/pulse
Source0:        %{name}-%{version}.tar.gz

BuildRequires:  gcc
BuildRequires:  make

%description
Pulse is a lightweight Linux process and system monitoring tool.

%prep
%autosetup

%build
%make_build

%install
%make_install destdir=%{buildroot} prefix=%{_prefix}

%files
%license LICENSE
%doc README.md
%{_bindir}/pulse
%{_datadir}/bash-completion/completions/pulse

%changelog
