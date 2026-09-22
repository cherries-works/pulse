Name:           pulse
Version:        0.4.1
Release:        1%{?dist}
Summary:        Lightweight Linux process monitoring tool

License:        MIT
Source0:        pulse-0.4.1.tar.gz

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

%changelog
* Sun Sep 20 2026 Mert <mail@mertk.uk> - 0.4.1
- Initial RPM package