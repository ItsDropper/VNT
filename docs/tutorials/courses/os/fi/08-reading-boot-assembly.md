# Käynnistys-assemblyn lukeminen

Copyright (c) 2026 ItsDropper. MIT-lisenssi; katso projektin juuren `LICENSE`.

Assembly näyttää konekäskyt hyvin tarkasti. VNTOSissa sitä käytetään käynnistykseen, keskeytysten sisääntuloon ja toissijaisen CPU:n käynnistysalustaan. Lue `os/arch/x86_64/boot.S` pienissä osissa. Seuraa tunnisteita, rekistereitä, pinoa, hyppyjä ja muistiosoitteita.

Yksittäisen käskyn merkitys riippuu aiemmasta CPU:n tilasta ja edeltävistä käskyistä. Assemblyn pitäisi pysyä pienenä: tavallinen päätöksenteko kuuluu VNT-koodiin, jos se voidaan toteuttaa siellä turvallisesti.

**Harjoitus:** kirjaa viisi tuntematonta käskyä ja selvitä, mitä arvoa tai tilaa kukin muuttaa.

---

[← Sisällysluettelo](./README.md) · [← Edellinen: GRUB ja Multiboot2](./07-grub-and-multiboot2.md) · [Seuraava oppitunti: x86-64 long mode →](./09-long-mode.md)
