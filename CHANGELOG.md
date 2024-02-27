# Changelog
All notable changes to KDisplay will be documented in this file.
## [6.0.0](https://github.com/winft/kdisplay/compare/kdisplay@5.27.0-beta.0...v6.0.0) (2024-02-27)


### Features

* force plasma theme in OSD ([9801592](https://github.com/winft/kdisplay/commit/98015929778c97be8ccdd56bbbea8e544129df90))
* **plasmoid:**  use Switch for instant-apply "presentation mode" setting ([f9748c9](https://github.com/winft/kdisplay/commit/f9748c90f0a699674d9a6f77d05148017f03c570))
* **plasmoid:** only show if the user enabled presentation mode ([1fb4065](https://github.com/winft/kdisplay/commit/1fb406505ff719c5a99c0555cd6abe27083c9837))
* support adaptive sync ([f579f7e](https://github.com/winft/kdisplay/commit/f579f7e08be36763df595971d31d83ae9f9e35d3))
* use Kirigami heading ([d10f040](https://github.com/winft/kdisplay/commit/d10f0405a4e6098d36477ee1311e229168793923))
* use Kirigami units ([d567034](https://github.com/winft/kdisplay/commit/d5670344ffbb0be6fb66a945f687b7f898dcb22c))


### Bug Fixes

* adapt to kded6 renaming ([dc002dc](https://github.com/winft/kdisplay/commit/dc002dc285efd57b7077f0aa8b56c5cba6b6ebe3))
* adapt to Plasma API changes ([b2c31aa](https://github.com/winft/kdisplay/commit/b2c31aa24dc23508edff483db2f909a2593ca572))
* include declarations ([dba1473](https://github.com/winft/kdisplay/commit/dba1473ef9aede16ad68798a59b16b512564f035))
* **plasmoid:** correct metadata id ([8b27155](https://github.com/winft/kdisplay/commit/8b271557c5ee05db3b780227fcab32e2cf4e111f))
* **plasmoid:** register type in the ctor ([7d46843](https://github.com/winft/kdisplay/commit/7d46843dad243de5f9d0022d3869c933e069d39e))
* port to new action API ([e6519b1](https://github.com/winft/kdisplay/commit/e6519b1a0a337e0153d6b61ec019ba873b5b0d96))
* use Plasma5Support data source ([067ccdf](https://github.com/winft/kdisplay/commit/067ccdf65d1e5499d6c764dcc5e8c08baaade288))


### Refactors

* add parameters to source handlers ([1d560f0](https://github.com/winft/kdisplay/commit/1d560f0f6ca98bee78b751c9d60218d68f27516a))
* copy InhibitionHint from battery applet ([657eaa6](https://github.com/winft/kdisplay/commit/657eaa66524953d7ab5ae8166b561d2142437a90))
* modernize QML code to make it work with Qt 6 ([ff194dc](https://github.com/winft/kdisplay/commit/ff194dc1a1cffd296652cd0f4cf21969d51c258d))
* **plasmoid:** use internal enum ([870e410](https://github.com/winft/kdisplay/commit/870e4106f8e9bdacd1d09dbaa90c88133c7e7a1d))
* port to KConfig and KCMLauncher ([793cdfe](https://github.com/winft/kdisplay/commit/793cdfe0a771f6bf8d883ad57ad36af4e8ebd339))
* port to KSvg ([28708fb](https://github.com/winft/kdisplay/commit/28708fb05f9f567b6e4268cd81e7ea1651e0ab5d))
* port to PlasmaComponents3 ([82eecc0](https://github.com/winft/kdisplay/commit/82eecc0e4e755d45b312f23b7910986a4255cd39))
* provide osd as separate application ([e9ae432](https://github.com/winft/kdisplay/commit/e9ae432f633fd21a4a0723f8348fb033496ad257))
* remove output connected signals ([4785a15](https://github.com/winft/kdisplay/commit/4785a159a94e21a453a44e16af90155694a96b74))
* use K_PLUGIN_CLASS directly ([50fbf43](https://github.com/winft/kdisplay/commit/50fbf43875a42cca0dd612ce1fb89d8dd4c8d5be))
* use Kirigami font theme ([543b218](https://github.com/winft/kdisplay/commit/543b2181be85b5a35a38dc8a0ae5155ed00dbb75))
* use Kirigami icon ([a5f8bfc](https://github.com/winft/kdisplay/commit/a5f8bfc03db724efd5a84b3682daf31f67aaef91))
* use KX11Extras ([28f0ec1](https://github.com/winft/kdisplay/commit/28f0ec181b8c9efc4ea89e72f6bd1cc31059a258))
* use Qt 6 ([9abba36](https://github.com/winft/kdisplay/commit/9abba3635c3342ebbe294a8027f47f2bc2bfc7d2))
* use unversioned plasma core import ([47a0c6c](https://github.com/winft/kdisplay/commit/47a0c6cd2ca3a3073093867ccb0db02dacf0c442))

## [5.27.0](https://gitlab.com/kwinft/kdisplay/compare/kdisplay@5.27.0-beta.0...kdisplay@5.27.0) (2023-02-17)

## [5.27.0-beta.0](https://gitlab.com/kwinft/kdisplay/compare/kdisplay@5.26.0-beta.0...kdisplay@5.27.0-beta.0) (2023-02-16)

## [5.26.0](https://gitlab.com/kwinft/kdisplay/compare/kdisplay@5.26.0-beta.0...kdisplay@5.26.0) (2022-10-11)

## [5.26.0-beta.0](https://gitlab.com/kwinft/kdisplay/compare/kdisplay@5.25.0-beta.0...kdisplay@5.26.0-beta.0) (2022-10-10)

## [5.25.0](https://gitlab.com/kwinft/kdisplay/compare/kdisplay@5.25.0-beta.0...kdisplay@5.25.0) (2022-06-14)

## [5.25.0-beta.0](https://gitlab.com/kwinft/kdisplay/compare/kdisplay@5.24.0-beta.0...kdisplay@5.25.0-beta.0) (2022-06-12)


### Features

* hint to run single instance only ([44a47d4](https://gitlab.com/kwinft/kdisplay/commit/44a47d46276354c10e2841b1531e2bca68d31045))
* **kcm:** make orientation icons more obvious ([bb73b8b](https://gitlab.com/kwinft/kdisplay/commit/bb73b8b9175cc937dd36804622b3336abdacf5f7))
* **kcm:** show on mobile ([6f6fea6](https://gitlab.com/kwinft/kdisplay/commit/6f6fea678ac8abe4b8bc596ea24bef3e355d2739))


### Bug Fixes

* compiler warning TRANSLATION_DOMAIN redefined ([5253335](https://gitlab.com/kwinft/kdisplay/commit/525333526c0326a63b1435563b1a29c8f9981b2b))
* **kcm:** a typo causing warnings ([e70a5d0](https://gitlab.com/kwinft/kdisplay/commit/e70a5d0c5d410f011e52273a02141bd0cc967d5a))
* **kcm:** add correct spacing to the swipeview ([df8fa0b](https://gitlab.com/kwinft/kdisplay/commit/df8fa0b312f3f69bd69bccd3675d770dc839256b))
* **kcm:** connections warnings ([cf8d7cb](https://gitlab.com/kwinft/kdisplay/commit/cf8d7cb1d2a691d692e94f92a28244878c09cf70))
* **kcm:** error while loading the QML-file ([9b40ffa](https://gitlab.com/kwinft/kdisplay/commit/9b40ffa680d56a910a11ac10ae3326a4dee52ede))
* **kcm:** import QtQuick 2.15 ([cbbfb0a](https://gitlab.com/kwinft/kdisplay/commit/cbbfb0a12d46caa395bd8726b7941fd7e0e42715))
* **kcm:** limit the width of the scaling slider ([5c3dbd7](https://gitlab.com/kwinft/kdisplay/commit/5c3dbd7aae47ac5f354828bf3d86534c028c27c6))
* **kcm:** removal of Xft.dpi from Xresources ([57ca530](https://gitlab.com/kwinft/kdisplay/commit/57ca5306f8e9c6b7a0541572eb8b8df4f29c47a2))
* **kcm:** replace qmlRegisterType with qmlRegisterAnonymousType ([74dbb04](https://gitlab.com/kwinft/kdisplay/commit/74dbb04487c846ecd256e50b64ee1e0ed7b7ec44))
* **kcm:** replace valueFromText for scale spinboxes ([5e05507](https://gitlab.com/kwinft/kdisplay/commit/5e05507947e4c15306d00297a43d99b1797b6cf1))
* **kcm:** split up QProcess arguments ([ec882ac](https://gitlab.com/kwinft/kdisplay/commit/ec882acec0bf79f1e0da85c219586000ebb551ec))
* **kcm:** warning open build service RPM_Lint ([fafcfa9](https://gitlab.com/kwinft/kdisplay/commit/fafcfa97652fecff74c2bdbb6aff3df1fba18f16))
* plasmoid/kded wrong TRANSLATION_DOMAIN name ([2644623](https://gitlab.com/kwinft/kdisplay/commit/26446239632934df5ee9b46e0933cf073510eb98))
* **plasmoid:** change arguments for KDisplayApplet ([6560437](https://gitlab.com/kwinft/kdisplay/commit/6560437593a64ab74909a6c2e6fa457aa75c8895))
* **plasmoid:** modernize applet configuration UI ([2048abf](https://gitlab.com/kwinft/kdisplay/commit/2048abfd89197605dae44c64c4ecbff7c9b71239))


### Refactors

* **kcm/plasmoid:** replace emit with Q_EMIT ([c0362f3](https://gitlab.com/kwinft/kdisplay/commit/c0362f395aa01655b61186099e437bb3231d298f))
* **kcm:** only show auto-rotate options when sensor is available ([cada31f](https://gitlab.com/kwinft/kdisplay/commit/cada31f2862bf821e3737729da53a82d28cdcbe9))
* **kcm:** port away from KAboutData and KPackage metadata ([aa966d1](https://gitlab.com/kwinft/kdisplay/commit/aa966d1f82dd85573f7bff8ca67557f849ddf5c2))
* **kcm:** prefer 21:9 over 64:27 aspect ratio ([9a947b3](https://gitlab.com/kwinft/kdisplay/commit/9a947b3caff31fbdfcb43ded90f29c27f9af278a))
* **kcm:** reduce margin between screens previews and form ([5886cfe](https://gitlab.com/kwinft/kdisplay/commit/5886cfef7a9756daabdedff3d58f6dfb490a2047))
* **kcm:** remove Q_EMIT placed on non-signals ([7e3901b](https://gitlab.com/kwinft/kdisplay/commit/7e3901b69135602547f6f666eb54b8a485197ac5))

## [5.24.0](https://gitlab.com/kwinft/kdisplay/compare/kdisplay@5.24.0-beta.0...kdisplay@5.24.0) (2022-02-08)

## [5.24.0-beta.0](https://gitlab.com/kwinft/kdisplay/compare/kdisplay@5.23.0-beta.0...kdisplay@5.24.0-beta.0) (2022-02-03)

## [5.23.0](https://gitlab.com/kwinft/kdisplay/compare/kdisplay@5.23.0-beta.0...kdisplay@5.23.0) (2021-10-14)

## [5.23.0-beta.0](https://gitlab.com/kwinft/kdisplay/compare/kdisplay@5.22.0-beta.0...kdisplay@5.23.0-beta.0) (2021-10-06)

## [5.22.0](https://gitlab.com/kwinft/kdisplay/compare/kdisplay@5.22.0-beta.0...kdisplay@5.22.0) (2021-06-08)

## [5.22.0-beta.0](https://gitlab.com/kwinft/kdisplay/compare/kdisplay@5.21.0-beta.0...kdisplay@5.22.0-beta.0) (2021-05-26)


### Bug Fixes

* **kded:** pass the action id directly ([f7d0be0](https://gitlab.com/kwinft/kdisplay/commit/f7d0be077355894eb4a80a323999ba885ab4508f))
* **kded:** reference modelData.action directly ([0925338](https://gitlab.com/kwinft/kdisplay/commit/092533807f0e0e4f6d09aae3b0a23b566b3883c5))


### Refactors

* **kded:** use Layout ([738051d](https://gitlab.com/kwinft/kdisplay/commit/738051d045cc3ced1eb4af8e3cf9565f29841ce7))

## [5.21.0](https://gitlab.com/kwinft/kdisplay/compare/kdisplay@5.21.0-beta.0...kdisplay@5.21.0) (2021-02-16)

## [5.21.0-beta.0](https://gitlab.com/kwinft/kdisplay/compare/kdisplay@5.20.0-beta.0...kdisplay@5.21.0-beta.0) (2021-02-07)


### Bug Fixes

* use display notion in UI consistently ([04bc0aa](https://gitlab.com/kwinft/kdisplay/commit/04bc0aa8af8dcea1b06fdc6cd2e3279174a53e8f))
* **kcm:** generate pot-file relative to source root directory ([d818209](https://gitlab.com/kwinft/kdisplay/commit/d8182094834b9374b4723ccf1fb89e30e6d24d12))
* **kcm:** improve some UI strings ([3475d17](https://gitlab.com/kwinft/kdisplay/commit/3475d17c66405cacbc49b3fe75662a4301035dfe))
* **kcm:** snap to other outputs without gap ([cb3f985](https://gitlab.com/kwinft/kdisplay/commit/cb3f985b1e59be6a754ee71df2ec5d9de46a8799))

## [5.20.0](https://gitlab.com/kwinft/kdisplay/compare/kdisplay@5.20.0-beta.1...kdisplay@5.20.0) (2020-10-13)

## [5.20.0-beta.1](https://gitlab.com/kwinft/kdisplay/compare/kdisplay@5.20.0-beta.0...kdisplay@5.20.0-beta.1) (2020-10-02)


### Bug Fixes

* **kcm:** snap to other outputs without gap ([b9e18f8](https://gitlab.com/kwinft/kdisplay/commit/b9e18f8cd3f5da90dff4c1107477c45a56641710))

## [5.20.0-beta.0](https://gitlab.com/kwinft/kdisplay/compare/kdisplay@5.19.0-beta.0...kdisplay@5.20.0-beta.0) (2020-09-25)


### Features

* **kcm:** configure automatic resolution and refresh rate ([e6ec1be](https://gitlab.com/kwinft/kdisplay/commit/e6ec1be8f360ddf3b922f4f6f04139fffb79ec26))
* **kcm:** improve the output identifier design ([444d86b](https://gitlab.com/kwinft/kdisplay/commit/444d86b1d26b3bbd382817559aa69bcf5733bfb5))
* **kcm:** provide a standalone app ([05cc25d](https://gitlab.com/kwinft/kdisplay/commit/05cc25d2206653176338536545c44e47def3bc60))
* disable KDisplay's config control ([f17f3b5](https://gitlab.com/kwinft/kdisplay/commit/f17f3b547f01a05a8dc465344fe25c5137816d3b))
* **kcm:** control auto rotation with a switch ([212eb8f](https://gitlab.com/kwinft/kdisplay/commit/212eb8fa36a5c8efba2178515b648fcbe4c4d540))
* **kcm:** enlarge spacing between control element groupings ([0200d8a](https://gitlab.com/kwinft/kdisplay/commit/0200d8a39503f381ab1126ef78c964bf0237c551))
* **kcm:** move resolution setting down ([a50441e](https://gitlab.com/kwinft/kdisplay/commit/a50441e835e3e14271c46e5bbafb985e4aca8180))
* **kcm:** round displayed automatic refresh rates ([5fe96a3](https://gitlab.com/kwinft/kdisplay/commit/5fe96a361537ac824820cbc70bb13b3a7151cef1))


### Bug Fixes

* **kcm:** increase implicit size ([524799d](https://gitlab.com/kwinft/kdisplay/commit/524799d2a9acd53dfd088ff3ad74166d2cbe2ae5))
* **kded:** apply config again ([be7d584](https://gitlab.com/kwinft/kdisplay/commit/be7d584d1789b87babea19e568920ec9e23cbb4d))
* **kded:** monitor for changes from the start ([e83855c](https://gitlab.com/kwinft/kdisplay/commit/e83855c9fd95c918b9f98f6e79add10be0d8d54a))
* **kded:** send config auto rotation change ([81bbacf](https://gitlab.com/kwinft/kdisplay/commit/81bbacfdf86ac94d8f8166fbd699ede472629378))
* **kded:** set orientation sensor enablement on config change ([d44bbeb](https://gitlab.com/kwinft/kdisplay/commit/d44bbebdf8dbc65c01c576732d1094cc0e62bf90))
* **kded:** update orientation on every change ([96f4847](https://gitlab.com/kwinft/kdisplay/commit/96f48477013e75188cba0e1afe7a2c19475164be))
* replace deprecated QTextStream endl with Qt::endl ([0e194aa](https://gitlab.com/kwinft/kdisplay/commit/0e194aa0f0f89a05a536bd740578bc203045edab))


### Refactors

* **kded:** use osd action enum in display switch generation ([386103b](https://gitlab.com/kwinft/kdisplay/commit/386103bed9e87927928097f5ff9b1f0cbdbafaca))
* remove ambiguous Disman namespacing ([c5959a8](https://gitlab.com/kwinft/kdisplay/commit/c5959a890c067744c4b0332cb0a0025e26ce30e4))
* **kcm:** remove superfluous column layout ([621cff0](https://gitlab.com/kwinft/kdisplay/commit/621cff096b3433de46468b6ee155e031f16e4f1d))
* **kcm:** replace Plasma Dialog with QQuickView ([5c49f80](https://gitlab.com/kwinft/kdisplay/commit/5c49f80edc37ad0c2ca3548829c7b62d901ce2ac))
* **kcm:** replace Plasma QML elements with Kirigami ([2f8f5b5](https://gitlab.com/kwinft/kdisplay/commit/2f8f5b5972c6793215e6642792382c8446954e2c))
* **kded:** connect late to orientation sensor ([a8ba9dd](https://gitlab.com/kwinft/kdisplay/commit/a8ba9dd6c81e72d933f8a392109b0de66e935a7f))
* **kded:** connect to output changed signals in init directly ([5d28c47](https://gitlab.com/kwinft/kdisplay/commit/5d28c47d0c1f25c1d2fb3fcb5ba5986994139d92))
* **kded:** do not define dtor ([f7b6293](https://gitlab.com/kwinft/kdisplay/commit/f7b62935f588464fcccebbe0e4b17e51eda2c2c8))
* **kded:** early check for outputs count on display switch ([7347322](https://gitlab.com/kwinft/kdisplay/commit/73473226d648794ca2d5b1855a4f8803dde4d582))
* **kded:** get initial config directly ([418e075](https://gitlab.com/kwinft/kdisplay/commit/418e0753b65de1b627b9485463116c12ec65d16d))
* **kded:** hand over config operation to init method ([5ce10bf](https://gitlab.com/kwinft/kdisplay/commit/5ce10bf10423368e1a3eb5ab73eb756c02815b4c))
* **kded:** modernize Generator class ([27fa063](https://gitlab.com/kwinft/kdisplay/commit/27fa063be10d32903375ed2d1c53e0bf749013eb))
* **kded:** rely on auto-mode for generating optimal ones ([cfc067f](https://gitlab.com/kwinft/kdisplay/commit/cfc067f7324f150fe077f79c8d47ddb65526a4f2))
* **kded:** remove change compressor ([61a16eb](https://gitlab.com/kwinft/kdisplay/commit/61a16eb293b6cf909366431fb69a421866995f28))
* **kded:** remove config file handling ([b586c07](https://gitlab.com/kwinft/kdisplay/commit/b586c07e3cc513d0d67a306bb2f9906a25080a06))
* **kded:** remove Device class ([e015711](https://gitlab.com/kwinft/kdisplay/commit/e015711faa92bb4c6f6e62b4993aae69f323e245))
* **kded:** remove ideal and laptop config generation ([88f35f4](https://gitlab.com/kwinft/kdisplay/commit/88f35f4645987c6cf982297c700f84ad9c688c9c))
* **kded:** remove lid closed logic ([e2a1cde](https://gitlab.com/kwinft/kdisplay/commit/e2a1cde4863b4d8ccf1df5f8b1bed07ea6319e0f))
* **kded:** remove output file handling ([920c8c7](https://gitlab.com/kwinft/kdisplay/commit/920c8c78a9a5f29ca1eb13d95e1b82818e84d4ce))
* **kded:** remove unused functions ([29c1da3](https://gitlab.com/kwinft/kdisplay/commit/29c1da360cceb52a8f3dd481373d08a6de9cc5d3))
* **kded:** simplify Config class ([a4db7c6](https://gitlab.com/kwinft/kdisplay/commit/a4db7c6af994f424bd5c0bcc61a73c05ba6f02c0))
* **kded:** transform Generator to a namespace ([21a7899](https://gitlab.com/kwinft/kdisplay/commit/21a7899f82ceed13ddac05fd58b3f9387ffc91ec))
* adapt to Disman API changing to snake case ([a222831](https://gitlab.com/kwinft/kdisplay/commit/a2228317c663815fcb64d764f364e3161c916378))
* adapt to Disman API output clones property removal ([f74db78](https://gitlab.com/kwinft/kdisplay/commit/f74db7820e8d228dcc44a111b65cb620fedadb40))
* adapt to Disman API output connected property removal ([bee123f](https://gitlab.com/kwinft/kdisplay/commit/bee123ff5f478b3169aa1686581c5c0a29925a1b))
* adapt to Disman header names change ([7d7c408](https://gitlab.com/kwinft/kdisplay/commit/7d7c408662070aef08b127b8c9c1319dc2676905))
* adapt to Disman Output API hash function replacement ([dbdda68](https://gitlab.com/kwinft/kdisplay/commit/dbdda68afd5934e982836af21db3efe13b4d7c25))
* adapt to Disman::Edid string changes ([8151704](https://gitlab.com/kwinft/kdisplay/commit/81517041ba5980bce2b6a68916a4a6af735d1793))
* adapt to Disman::Mode refresh rate type change ([fc309ff](https://gitlab.com/kwinft/kdisplay/commit/fc309ff74806e3e6c1eb936599149d088cc1851c))
* adapt to Disman's control integration ([3dc9e86](https://gitlab.com/kwinft/kdisplay/commit/3dc9e86f38bce68d464175df26fe2d70b340d82b))
* adapt to Disman's primary output API change ([76ea067](https://gitlab.com/kwinft/kdisplay/commit/76ea067386ea0b600a4635fc3a050a07e214f59d))
* adapt to Disman's redesigned output modes API ([b4c29ed](https://gitlab.com/kwinft/kdisplay/commit/b4c29ed39b2c5bb244f2bb48fdaaf05de53867a7))
* adapt to Disman's slimmed down output API ([31fa871](https://gitlab.com/kwinft/kdisplay/commit/31fa871aaf71c11200085eb2aca63d19a798d2a9))
* adapt to Edid removal in Disman Output API ([1e5d2b3](https://gitlab.com/kwinft/kdisplay/commit/1e5d2b3d705ab9571409ec5be9d2c23c9e814a09))
* adapt to OutputList as std::map ([296eef8](https://gitlab.com/kwinft/kdisplay/commit/296eef8aa2c6b0050ffaf1900e82d221c44736c3))
* adapt to OutputMap and ModeMap renaming ([e093d12](https://gitlab.com/kwinft/kdisplay/commit/e093d12ee07030d4766eaaac3bb7413bae7bac8d))
* adapt to refresh rates in mHz ([d9e07ff](https://gitlab.com/kwinft/kdisplay/commit/d9e07ffeb893eb85f0cee19512dbc6df7dcf0013))
* adapt to renaming of cause concept ([00a519c](https://gitlab.com/kwinft/kdisplay/commit/00a519cf67650556fdb07161037872824d39cd4f))
* adapt to std::string usage in Disman Output hash ([c9ef650](https://gitlab.com/kwinft/kdisplay/commit/c9ef650a5742cd6f737a498db63cabb175d79b76))
* adapt to std::string usage in Disman Output name ([c9439dc](https://gitlab.com/kwinft/kdisplay/commit/c9439dc8a594ac337b898de7becab0af77c81cdc))
* adapt to std::string usage in Disman::Mode class ([abc4ecc](https://gitlab.com/kwinft/kdisplay/commit/abc4eccbba17eba156b44125b3febe263ea11b8a))
* adapt to types using std::shared_ptr ([ad2c9bc](https://gitlab.com/kwinft/kdisplay/commit/ad2c9bcf5288e02c58de0a03f863d543e6f92080))
* do not check on refresh rate index ([a3f3540](https://gitlab.com/kwinft/kdisplay/commit/a3f354030111da03b7f34bb4e5992679c008daee))
* remove common globals ([4c1e1aa](https://gitlab.com/kwinft/kdisplay/commit/4c1e1aa1ff12f196dacceffe292f457674c0adb8))
* remove kdisplay-console ([bf13cc6](https://gitlab.com/kwinft/kdisplay/commit/bf13cc6272b235569bf3ec8f8588d6500203c360))
* use Disman Config outputs getter only ([e3700a1](https://gitlab.com/kwinft/kdisplay/commit/e3700a1789fb549b53902d9848d67ddde5d3f502))
* **kded:** use new generator in Disman ([6ebda2c](https://gitlab.com/kwinft/kdisplay/commit/6ebda2c50e2899e99f7f8813c187d27429acda70))

## [5.19.0](https://gitlab.com/kwinft/kdisplay/compare/kdisplay@5.19.0-beta.0...kdisplay@5.19.0) (2020-06-09)

## [5.19.0-beta.0](https://gitlab.com/kwinft/kdisplay/compare/kdisplay@0.0.0...kdisplay@5.19.0-beta.0) (2020-05-24)


### Features

* write configuration files with .json name extension ([51bc9d9](https://gitlab.com/kwinft/kdisplay/commit/51bc9d9bf3869f73c2c277e0b9bea03fcd351fe2))


### Bug Fixes

* **kded:** when no lid-open file exist apply default config ([187bedf](https://gitlab.com/kwinft/kdisplay/commit/187bedf873b076bdd36b050d789084cff5eba42b))
* fall back to scale 1 ([92e11c1](https://gitlab.com/kwinft/kdisplay/commit/92e11c1feed92b849760db611cf9ff059dddb20e))
* on null Config abort in Control ctor ([8d1e7f7](https://gitlab.com/kwinft/kdisplay/commit/8d1e7f7db5e375933ec46044d0c02edd3e919e2b))
* **kded:** set scale by control file ([fc2f875](https://gitlab.com/kwinft/kdisplay/commit/fc2f875e9257bfca0b5701e10d00801da00af016))


### Refactors

* **kded:** change config save path ([bdf4642](https://gitlab.com/kwinft/kdisplay/commit/bdf4642aaf64fc616faa093eabf10e84a920c71c))
* adapt to recent Disman API changes ([815b953](https://gitlab.com/kwinft/kdisplay/commit/815b953a4680575767384aa9a9869c3bf71a2d3b))
* rename project ([6052263](https://gitlab.com/kwinft/kdisplay/commit/60522631dbe20beb44dfc52c6c3575de827af9de))
* save config in map ([5700b25](https://gitlab.com/kwinft/kdisplay/commit/5700b25a169b36468a8ab5ec83abffced6b95431))
