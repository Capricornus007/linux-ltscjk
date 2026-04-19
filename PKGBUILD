# Maintainer: Capricornus007 <sihaogang@gmail.com>

pkgbase=linux-ltscjk
pkgname=("$pkgbase" "$pkgbase-headers")
pkgver=6.12.82
pkgrel=1
pkgdesc='Linux LTS (with cjktty-patches)'
url='https://www.kernel.org'
arch=(x86_64)
license=(GPL-2.0-only)
makedepends=(
  bc cpio gettext libelf pahole perl python
  tar xz
)
options=(!debug !strip)

_srcname="linux-$pkgver"
_cjktty_repo="https://github.com/bigshans/cjktty-patches"

source=(
  "https://cdn.kernel.org/pub/linux/kernel/v6.x/linux-${pkgver}.tar.xz"
  "https://cdn.kernel.org/pub/linux/kernel/v6.x/linux-${pkgver}.tar.sign"
  "config"  # 這是從你的 config_base 複製過來的檔案
  "0001-cjktty.patch::$_cjktty_repo/raw/master/v6.x/cjktty-6.12.63.patch"
  "0002-cjktty-font.patch::$_cjktty_repo/raw/master/cjktty-add-cjk32x32-font-data.patch"
)

# 執行 updpkgsums 會自動填寫這裡
sha256sums=('SKIP' 'SKIP' 'SKIP' 'SKIP' 'SKIP')

validpgpkeys=(
  'ABAF11C65A2970B130ABE3C479BE3E4300411886' # Linus Torvalds
  '647F28654894E3BD457199BE38DBBDC86092693E' # Greg Kroah-Hartman
)

prepare() {
  cd "$_srcname"

  echo "Applying CJKTTY patches..."
  patch -Np1 < "../0001-cjktty.patch"
  patch -Np1 < "../0002-cjktty-font.patch"

  echo "Setting config..."
  cp ../config .config

  # 將 6.18 的 config 適配到 6.12
  make olddefconfig

  make -s kernelrelease > version
}

build() {
  cd "$_srcname"
  make all
}

_package() {
  pkgdesc="The $pkgdesc kernel and modules"
  depends=(coreutils initramfs kmod)
  optdepends=(
    'linux-firmware: firmware images needed for some devices'
    'wireless-regdb: to set the correct wireless channels of your country'
  )
  provides=(
    KSMBD-MODULE
    VIRTUALBOX-GUEST-MODULES
    WIREGUARD-MODULE
  )

  cd "$_srcname"
  local modulesdir="$pkgdir/usr/lib/modules/$(<version)"

  echo "Installing boot image..."
  install -Dm644 "$(make -s image_name)" "$modulesdir/vmlinuz"

  echo "$pkgbase" | install -Dm644 /dev/stdin "$modulesdir/pkgbase"

  echo "Installing modules..."
  ZSTD_CLEVEL=19 make INSTALL_MOD_PATH="$pkgdir/usr" INSTALL_MOD_STRIP=1 \
    DEPMOD=/doesnt/exist modules_install

  # 移除 build 鏈接，編譯機與目標機路徑不同，必須移除
  rm "$modulesdir"/build
}

_package-headers() {
  pkgdesc="Headers and scripts for building modules for the $pkgdesc kernel"
  depends=(pahole)

  cd "$_srcname"
  local builddir="$pkgdir/usr/lib/modules/$(<version)/build"

  echo "Installing build files..."
  install -Dt "$builddir" -m644 .config Makefile Module.symvers System.map \
    localversion.* version vmlinux tools/bpf/bpftool/vmlinux.h
  install -Dt "$builddir/kernel" -m644 kernel/Makefile
  install -Dt "$builddir/arch/x86" -m644 arch/x86/Makefile
  cp -t "$builddir" -a scripts

  # 必要工具
  install -Dt "$builddir/tools/objtool" tools/objtool/objtool
  install -Dt "$builddir/tools/bpf/resolve_btfids" tools/bpf/resolve_btfids/resolve_btfids

  echo "Installing headers..."
  cp -t "$builddir" -a include
  cp -t "$builddir/arch/x86" -a arch/x86/include
  install -Dt "$builddir/arch/x86/kernel" -m644 arch/x86/kernel/asm-offsets.s

  # 驅動頭文件
  install -Dt "$builddir/drivers/md" -m644 drivers/md/*.h
  install -Dt "$builddir/net/mac80211" -m644 net/mac80211/*.h
  install -Dt "$builddir/drivers/media/i2c" -m644 drivers/media/i2c/msp3400-driver.h
  install -Dt "$builddir/drivers/media/usb/dvb-usb" -m644 drivers/media/usb/dvb-usb/*.h
  install -Dt "$builddir/drivers/media/dvb-frontends" -m644 drivers/media/dvb-frontends/*.h
  install -Dt "$builddir/drivers/media/tuners" -m644 drivers/media/tuners/*.h
  install -Dt "$builddir/drivers/iio/common/hid-sensors" -m644 drivers/iio/common/hid-sensors/*.h

  echo "Installing KConfig files..."
  find . -name 'Kconfig*' -exec install -Dm644 {} "$builddir/{}" \;

  echo "Installing Rust files..."
  if [ -d rust ]; then
    cp -t "$builddir" -a rust
  fi

  echo "Installing unstripped VDSO..."
  make INSTALL_MOD_PATH="$pkgdir/usr" vdso_install link=

  echo "Cleaning up..."
  local arch
  for arch in "$builddir"/arch/*/; do
    [[ $arch = */x86/ ]] && continue
    rm -r "$arch"
  done
  rm -rf "$builddir/Documentation"
  find -L "$builddir" -type l -delete
  find "$builddir" -type f -name '*.o' -delete

  strip -v $STRIP_STATIC "$builddir/vmlinux"

  echo "Adding symlink..."
  mkdir -p "$pkgdir/usr/src"
  ln -sr "$builddir" "$pkgdir/usr/src/$pkgbase"
}

# 自動分配 package 函數
for _p in "${pkgname[@]}"; do
  eval "package_$_p() {
    $(declare -f "_package${_p#$pkgbase}")
    _package${_p#$pkgbase}
  }"
done
