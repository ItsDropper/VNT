# VNTOS-kurssi suomeksi

Helppotajuinen suomenkielinen johdatus VNTOS-käyttöjärjestelmän ytimen tutkimiseen. Kurssi seuraa projektin nykyistä toteutusta eikä väitä keskeneräisiä ominaisuuksia valmiiksi.

## Ennen aloitusta
- Testaa vain QEMU-virtuaalikoneessa. Älä asenna keskeneräistä ydintä omalle tietokoneelle.
- Lue oppitunnit järjestyksessä. Jokaisen lopussa on klikattava linkki seuraavaan oppituntiin.
- Build-komento projektin juurihakemistosta: `bash os/build.sh`.
- Käännöksen onnistuminen ei yksin todista, että kernel käynnistyy tai toimii oikein.
- Jos dokumentaatio ja lähdekoodi ovat ristiriidassa, tarkista nykyinen toteutus ja CI-tulos.

## Oppitunnit

1. [Aloita tästä: ensimmäinen kernel-harjoitus](00-start-here.md)
2. [Mitä käyttöjärjestelmä tekee?](01-what-is-an-os.md)
3. [Tietokone kernelin näkökulmasta](02-computer-basics.md)
4. [Turvallisen kehitysympäristön valmistelu](03-setup.md)
5. [VNT-kielen perusteet kernel-koodissa](04-vnt-for-kernels.md)
6. [Miten lähdekoodista syntyy käynnistettävä kernel](05-build-pipeline.md)
7. [Mitä käynnistyksessä tapahtuu?](06-the-boot-sequence.md)
8. [GRUB ja Multiboot2](07-grub-and-multiboot2.md)
9. [Käynnistys-assemblyn lukeminen](08-reading-boot-assembly.md)
10. [x86-64 long mode](09-long-mode.md)
11. [Alkuvaiheen sivutaulut](10-initial-paging.md)
12. [ELF ja linkkeriskripti](11-elf-and-linker.md)
13. [Freestanding VNT -ohjelma](12-freestanding-vnt.md)
14. [Sarjaportin tulostus ja syöte](13-serial-io.md)
15. [Sarjaportin komentotulkki](14-the-serial-shell.md)
16. [Käynnistysongelmien selvittäminen](15-debugging.md)
17. [Testaa muutokset arvailun sijaan](16-testing.md)
18. [Ensimmäinen pieni kernel-muutos](17-first-kernel-change.md)
19. [Mihin jatkaa kurssin jälkeen?](18-next-steps.md)
20. [Jaa kernel VNT-moduuleihin](19-split-the-kernel-into-vnt-modules.md)
21. [Keskeytykset, poikkeukset ja IDT](20-interrupts-and-the-idt.md)
22. [Pidä assembly-rajapinta pienenä](21-minimal-assembly-entry-stubs.md)
23. [Keskeytysten käsittely VNT-kielellä](22-vnt-interrupt-dispatch.md)
24. [PIT, PIC ja ajastimen tikit](23-pit-pic-and-timer-ticks.md)
25. [Rakenna ja testaa keskeytyksiä käyttävä kernel](24-build-and-test-the-interrupt-kernel.md)
26. [Fyysinen muisti ja sivukehykset](26-physical-memory-and-page-frames.md)
27. [Loppuprojekti: paranna yhtä alijärjestelmää](25-graduation-project.md)
28. [ACPI, APIC ja usean CPU:n käynnistys](27-acpi-apic-and-smp.md)
29. [APIC IPI -viestit](28-apic-ipi-service.md)
30. [SMP-turvallisuus: synkronointi ennen yhteistä muistia](29-smp-synchronization-contract.md)

## Lähdekoodin kartta
- Käynnistys: `os/arch/x86_64/boot.S`
- Keskeytysten matalan tason rajapinta: `os/arch/x86_64/interrupts.S`
- Kernelin käynnistys: `os/kernel.vnt`
- Shell: `os/kernel/shell.vnt`
- Keskeytykset: `os/interrupts.vnt`
- Fyysinen muisti: `os/mm/phys.vnt`
- ACPI: `os/firmware/acpi.vnt`
- APIC ja SMP: `os/arch/x86_64/apic.vnt`, `os/kernel/smp.vnt`

