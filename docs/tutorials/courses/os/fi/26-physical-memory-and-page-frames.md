# Fyysinen muisti ja sivukehykset

Copyright (c) 2026 ItsDropper. MIT-lisenssi; katso projektin juuren `LICENSE`.

Kernel jakaa fyysisen muistin usein samankokoisiin sivukehyksiin. Tavallinen x86-64-sivukoko on 4096 tavua. Varaaja pitää kirjaa siitä, mitkä kehykset ovat käytettävissä ja mitkä on jo varattu.

VNTOSin varaaja on `os/mm/phys.vnt`-tiedostossa. Se käyttää bittikarttaa ja vapaiden kehysten laskuria. Tutki varaus- ja vapautusfunktiot. Nykyinen varaaja ei ole yleisesti turvallinen usean CPU:n samanaikaisessa käytössä; älä oleta muuta ennen lukituksen ja moniydintestien valmistumista.

**Pohdi:** Miksi samaa kehystä ei saa antaa kahdelle kutsujalle?

---

[← Sisällysluettelo](./README.md) · [← Edellinen: Rakenna ja testaa keskeytyksiä käyttävä kernel](./24-build-and-test-the-interrupt-kernel.md) · [Seuraava oppitunti: Loppuprojekti: paranna yhtä alijärjestelmää →](./25-graduation-project.md)
