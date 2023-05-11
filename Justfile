meson-exists:
	@if ! which meson 1>/dev/null 2>&1; then echo 'Cannot find meson executable'; exit 1; fi


alias sr := setup-release
setup-release: meson-exists
	meson setup build_release

alias sd := setup-debug
setup-debug: meson-exists
	meson setup build_debug

alias d := debug
debug: meson-exists setup-debug
	meson configure --buildtype debug --debug -Db_lundef=false --optimization 0 -Dcpp_debugstl=true -Db_sanitize=address,undefined --warnlevel 3 build_debug
	meson compile -C build_debug

alias r := release
release: meson-exists setup-release
	meson configure --buildtype release -Db_sanitize=none --optimization 3 -Db_lto=true -Dcpp_debugstl=false build_release
	meson compile -C build_release

# vim: ft=make
