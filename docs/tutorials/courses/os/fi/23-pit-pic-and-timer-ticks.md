# PIT, PIC ja ajastimen tikit

Copyright (c) 2026 ItsDropper. MIT-lisenssi; katso projektin juuren `LICENSE`.

PIT on perinteinen ajastin ja PIC keskeytysohjain. Ajastin voi lähettää säännöllisiä keskeytyksiä, joita kernel käyttää ajan mittaamiseen tai myöhemmin tehtävien ajoittamiseen.

Ajastintikki ei itsessään ole valmis tehtäväajastin. Tehtävien vaihtamiseen tarvitaan CPU-tilojen tallennus, ajastimen tietorakenteet ja selkeä keskeytyskäytäntö. Tutki `os/arch/x86_64/interrupts.S` ja `os/interrupts.vnt`; etsi alustus ja tikin käsittely.

**Pohdi:** Mitä ajastintikki kertoo? Miksi tikit eivät yksin muodosta tehtäväajastinta?

---

[← Sisällysluettelo](./README.md) · [← Edellinen: Keskeytysten käsittely VNT-kielellä](./22-vnt-interrupt-dispatch.md) · [Seuraava oppitunti: Rakenna ja testaa keskeytyksiä käyttävä kernel →](./24-build-and-test-the-interrupt-kernel.md)
