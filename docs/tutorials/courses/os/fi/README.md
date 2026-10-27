# VNTOS-kurssi suomeksi: opi rakentamaan käyttöjärjestelmän ydintä

Tämä on aloittelijan kurssi, joka etenee käsitteistä todelliseen toteutukseen. Jokaisen oppitunnin lopussa on klikattava linkki seuraavaan oppituntiin.

**Tärkeä lisäys:** [Avaa kattava toteutusopas](IMPLEMENTATION-HANDBOOK.md). Se käy läpi nykyisen `os/`-lähdekoodin, build-ketjun, käynnistyksen, muistinhallinnan, keskeytykset, ACPI:n, APICin ja SMP:n konkreettisina toteutusvaiheina. Lue se yhdessä oppituntien kanssa; älä tyydy vain käsitteiden lukemiseen.

## Näin opiskelet

1. Lue oppitunnin selitys ja sanasto.
2. Avaa nimetyt oikeat lähdekooditiedostot ja seuraa kutsuketjua.
3. Tee toteutusharjoitus: määritä rajapinta, muuta koodia, käännä ja testaa.
4. Testaa sekä normaali tapaus että virheellinen syöte tai rajatapaus.
5. Kirjaa tarkka havainto, joka todistaa toiminnon. Älä väitä testausta tehdyksi ilman tulosta.
6. Siirry seuraavaan oppituntiin sivun lopun linkistä.

## Turvallisuus ja testaus

- Käytä kokeisiin QEMU-virtuaalikonetta. Älä asenna keskeneräistä ydintä omalle tietokoneelle tai kirjoita levykuvaa oikealle USB-tikulle.
- Suorita build-komennot projektin juuresta, ellei ohje sano muuta.
- Selvitä ensimmäinen todellinen virhe. Myöhemmät virheet voivat olla sen seurauksia.
- Käännös ei todista käynnistymistä, eikä käynnistyminen todista kaikkien ominaisuuksien toimivan.
- Jos teksti ja lähdekoodi eroavat, tarkista nykyinen lähdekoodi ja testien todelliset tulokset.
- Pidä assembly mahdollisimman pienenä: CPU:n vaatima sisääntulo/paluu ja käynnistysmekanismi ovat arkkitehtuurikoodia; päätöksenteko kuuluu VNT-moduuleihin.

## Sanasto

- **Kernel / ydin:** käyttöjärjestelmän etuoikeutettu osa.
- **Lähdekoodi:** ohjelmoijan kirjoittama teksti.
- **Kääntäjä:** muuttaa lähdekoodin konekoodiksi tai välivaiheeksi.
- **Linkkeri:** yhdistää käännetyt osat ELF-kuvaksi.
- **QEMU:** jäljittelee tietokonetta ohjelmallisesti.
- **CI:** automaattiset tarkistukset palvelimella.
- **ABI:** konekoodin kutsusopimus, joka määrittää esimerkiksi rekisterit ja pinon.
- **IDT:** taulukko, jonka avulla x86-64 ohjaa keskeytykset käsittelijöihin.
- **Sivukehys:** fyysisen muistin 4096 tavun lohko.
- **MMIO:** laitteistorekisterien käyttäminen muistiosoitteiden kautta.
- **SMP:** usean prosessoriytimen käyttö.

## Oppitunnit

1. [Aloita tästä](00-start-here.md)
2. [Mitä käyttöjärjestelmä tekee?](01-what-is-an-os.md)
3. [Tietokoneen osat kernelin näkökulmasta](02-computer-basics.md)
4. [Valmistele kehitysympäristö](03-setup.md)
5. [VNT-kielen perusteet kernel-koodissa](04-vnt-for-kernels.md)
6. [Miten lähdekoodista syntyy kernel](05-build-pipeline.md)
7. [Mitä käynnistyksessä tapahtuu?](06-the-boot-sequence.md)
8. [GRUB ja Multiboot2](07-grub-and-multiboot2.md)
9. [Lue käynnistys-assemblya](08-reading-boot-assembly.md)
10. [x86-64 long mode](09-long-mode.md)
11. [Alkuvaiheen sivutaulut](10-initial-paging.md)
12. [ELF ja linkkeriskripti](11-elf-and-linker.md)
13. [Freestanding-ohjelma](12-freestanding-vnt.md)
14. [Sarjaportti debuggausvälineenä](13-serial-io.md)
15. [Sarjaportin komentotulkki](14-the-serial-shell.md)
16. [Selvitä ongelmat järjestelmällisesti](15-debugging.md)
17. [Testaa toimintaa, älä vain käännöstä](16-testing.md)
18. [Tee ensimmäinen pieni muutos](17-first-kernel-change.md)
19. [Miten jatkaa kurssin jälkeen](18-next-steps.md)
20. [Jaa kernel VNT-moduuleihin](19-split-the-kernel-into-vnt-modules.md)
21. [Keskeytykset, poikkeukset ja IDT](20-interrupts-and-the-idt.md)
22. [Pidä assembly-rajapinta pienenä](21-minimal-assembly-entry-stubs.md)
23. [Käsittele keskeytyksiä VNT-kielellä](22-vnt-interrupt-dispatch.md)
24. [PIT, PIC ja ajastimen tikit](23-pit-pic-and-timer-ticks.md)
25. [Rakenna ja testaa keskeytyskernel](24-build-and-test-the-interrupt-kernel.md)
26. [Loppuprojekti: tee todennettava parannus](25-graduation-project.md)
27. [Fyysinen muisti ja sivukehykset](26-physical-memory-and-page-frames.md)
28. [ACPI, APIC ja usea CPU-ydin](27-acpi-apic-and-smp.md)
29. [APIC IPI -viestit](28-apic-ipi-service.md)
30. [SMP-turvallisuus: synkronoi ennen jaettua muistia](29-smp-synchronization-contract.md)
31. [Paranna sivukehysten varaajaa next-fit-haulla](30-physical-allocator-next-fit.md)
32. [Varaa peräkkäisiä fyysisiä sivuja](31-contiguous-physical-pages.md)
33. [Anna varatuille muistialueille etusija](32-reserved-memory-map-precedence.md)
34. [Komentohistoria ja käyttöajan diagnostiikka](33-terminal-history-and-uptime.md)
35. [Rakenna kohdistinta tukeva komentorivieditori](34-terminal-editable-line-editor.md)
36. [Tulosta käyttöaika ja laskurit luettavina desimaalilukuina](35-terminal-decimal-uptime.md)
37. [Ylläpidä komentotulkin ohjetta ja testaa se QEMUssa](36-shell-help-output.md)
38. [Testaa desimaalimerkin poiminta ilman konsolitulostusta](37-decimal-digit-self-test.md)
39. [Erota IRQ-laskurit keskeytysohjaimen asetuksista](38-irq-controller-diagnostics.md)

## Lähdekoodin kartta

- `os/kernel.vnt` — kernelin käynnistys ja alustusjärjestys.
- `os/kernel/shell.vnt` — sarjaportin komentotulkki.
- `os/kernel/sync.vnt` — atomiseen vaihtoon perustuva spinlock; kilpailutilanteen turvallisuutta ei ole vielä todennettu.
- `os/interrupts.vnt` ja `os/arch/x86_64/interrupts.S` — keskeytyskäsittely.
- `os/arch/x86_64/boot.S` — käynnistyksen matalan tason koodi.
- `os/mm/phys.vnt` — fyysisten sivukehysten bittikartat, next-fit-haku ja yhtenäisten sivualueiden varaus.
- `os/mm/virt.vnt` — varhaisten sivutaulujen tarkastelu ja kartoitus VNT-kielellä.
- `os/mm/phys.vnt` — fyysisten sivukehysten varaaja.
- `os/mm/virt.vnt` — varhaisen virtuaalimuistin kartoituksen tarkastelu.
- `os/firmware/acpi.vnt` — ACPI-tiedot.
- `os/arch/x86_64/apic.vnt` ja `os/kernel/smp.vnt` — APIC ja moniydinkokeet.
- `os/build.sh` — build-ketju.
- `src/compiler/freestanding_x86.c` — freestanding x86 -kääntäjän toteutus.
- [Kattava toteutusopas](IMPLEMENTATION-HANDBOOK.md) — miten nämä osat rakennetaan ja todennetaan.
