Name:           pulse
Version:        %{_pulse_version}
Release:        1%{?dist}
Summary:        Lightweight Linux process monitoring tool

%global debug_package %{nil}

License:        MIT
URL:            https://github.com/cherries-works/pulse
Source0:        %{url}/archive/refs/tags/%{version}.tar.gz

BuildRequires:  gcc

%description
Pulse is a lightweight Linux process and system monitoring tool.

%prep
%autosetup

%build
make

%check
make test

%install
rm -rf %{buildroot}

make install destdir=%{buildroot} prefix=%{_prefix}

%files
%license LICENSE
%doc README.md
%{_bindir}/pulse
%{_datadir}/bash-completion/completions/pulse
