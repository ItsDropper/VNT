# x86-64 long mode

Copyright (c) 2026 ItsDropper. MIT-lisenssi; katso projektin juuren `LICENSE`.

x86-64-prosessori tarvitsee valmisteluja ennen 64-bittisen kernel-koodin suorittamista. Käynnistyskoodi asettaa tarvittavat CPU-tilat ja sivutaulut ja siirtyy sitten long mode -tilaan. Tämä ei vielä tarkoita, että kernelillä olisi valmis muistinhallinta tai ajurit.

Avaa `os/arch/x86_64/boot.S` ja seuraa siirtymää. Selvitä, mitä tehdään ennen 64-bittiseen koodiin siirtymistä ja mitä sen jälkeen.

**Pohdi:** Miksi long modeen ei voi hypätä ilman valmisteluja? Mikä ero on 64-bittisellä tilalla ja toimivalla muistinhallinnalla?

---

[← Sisällysluettelo](./README.md) · [← Edellinen: Käynnistys-assemblyn lukeminen](./08-reading-boot-assembly.md) · [Seuraava oppitunti: Alkuvaiheen sivutaulut →](./10-initial-paging.md)
