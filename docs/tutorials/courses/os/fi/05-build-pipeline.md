# Miten lähdekoodista syntyy käynnistettävä kernel

Copyright (c) 2026 ItsDropper. MIT-lisenssi; katso projektin juuren `LICENSE`.

VNTOSin build-ketju kääntää VNT- ja assembly-tiedostoja, yhdistää ne linkkerillä ja pakkaa käynnistystiedostot GRUBin ISO-levykuvaan. Avaa `os/build.sh` ja selvitä, mitä kukin vaihe lukee ja tuottaa.

Aja `bash os/build.sh`. Jos build epäonnistuu, etsi ensimmäinen varsinainen virhe. Myöhemmät linkkerivirheet voivat olla aiemman ongelman seurauksia. Pelkkä ISO-tiedoston olemassaolo ei todista, että se käynnistyy.

**Pohdi:** Mitkä ovat build-ketjun kolme vaihetta? Mikä on linkkerin tehtävä?

---

[← Sisällysluettelo](./README.md) · [← Edellinen: VNT-kielen perusteet kernel-koodissa](./04-vnt-for-kernels.md) · [Seuraava oppitunti: Mitä käynnistyksessä tapahtuu? →](./06-the-boot-sequence.md)
