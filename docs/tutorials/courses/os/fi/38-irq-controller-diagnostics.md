# Erota IRQ-laskurit keskeytysohjaimen asetuksista

Copyright (c) 2026 ItsDropper. Lisensoitu MIT-lisenssillä; katso projektin juuren `LICENSE`.

## Tavoite

Keskeytysdiagnostiikan pitää erottaa kolme eri asiaa:

1. **Asetukset:** mitkä keskeytykset ohjain tällä hetkellä peittää.
2. **Toimituslaskenta:** kuinka monta legacy PIC -vektoria päätyi VNT-dispatcheriin.
3. **Laitteen toimintakunto:** onko laite olemassa ja toimiiko se oikein.

Tämä varhainen kernel pystyy tarkastelemaan kahta ensimmäistä. Pelkästä keskeytyslaskurista ei voi päätellä kolmatta.

## Lue PIC:n todellinen maskibitti

Avaa `os/pic.vnt` ja etsi `pic_irq_is_masked(irq)`. Funktio tarkistaa IRQ-numeron, lukee master-PICin dataportin (`0x21`) IRQ-numeroille 0–7 tai slave-PICin dataportin (`0xA1`) numeroille 8–15, siirtää valitun bitin nollanteen bittipaikkaan ja palauttaa sen. Paluuarvo 1 tarkoittaa peitettyä keskeytystä, 0 peittämätöntä ja -1 virheellistä IRQ-numeroa.

Tämä on vain luku -diagnostiikka. Se ei ota keskeytystä käyttöön eikä poista sitä käytöstä. Toteutus käyttää moduulin muiden varhaisen kernelin rutiinien tavoin inline-assemblyn kutsukäytäntöä, joten kääntäjän ABI-muutosten yhteydessä pinon siirtymät on tarkistettava.

## Laajenna `irq`-komentoa

Tiedoston `os/kernel/shell.vnt` `irq`-komento raportoi:

- PIT-ajastimen tikit ja VNT-dispatcheriin kirjattujen legacy IRQ0/IRQ1 -vektorien määrät.
- IRQ0:n ja IRQ1:n todelliset PIC-maskibitit.
- Onko Local APIC alustettu.
- Valmistuiko tuettu I/O APICin maskausalustus.
- Kuinka monta I/O APICin reititystaulun merkintää alustus peitti.

Arvot vastaavat eri kysymyksiin. Nykyisessä asetuksessa IRQ0:n odotetaan olevan peittämätön ja kasvavan ajastimen toimiessa; IRQ1:n odotetaan olevan peitetty, koska näppäimistön syöte luetaan kyselyllä. I/O APICin valmiuslippu on tärkeä, koska nolla reititysmerkintää voi tarkoittaa myös sitä, ettei alustus valmistunut.

I/O APIC -rutiini peittää reitit sen sijaan, että se ottaisi käyttöön toteuttamattomia laitekeskeytyskäsittelijöitä. Alustettua ohjainta ei pidä tulkita todisteeksi siitä, että kernel reitittää laitekeskeytykset oikein.

## Regressiotarkistukset

QEMU-työnkulku suorittaa `irq`-komennon ja tarkistaa, että jokaisen uuden diagnostiikkatunnisteen jälkeen tulostuu desimaaliluku. Tämä todistaa, että komento tulosti arvon, ei sitä, että arvo olisi sama kaikilla koneilla. Laiteohjelmiston taulut, emuloitu laitteisto ja ohjaimen alustuksen polut voivat poiketa toisistaan.

Vahvempi tuleva testi voisi tarkistaa odotetut maskiarvot QEMU-kokoonpanossa, jossa PIC-asetukset ovat hallittuja ja deterministisiä. Älä muuta PIC-maskeja vain testausta varten: se voi poistaa ajastimen käytöstä tai paljastaa laitekeskeytyksen, jolle ei ole käsittelijää.

## Rajat

- Toimituslaskurit laskevat `interrupt_dispatch`-funktioon saapuneet vektorit; ne eivät laske tapahtumia, jotka jäivät toimittamatta.
- Laskurit käyttävät yksinkertaista jaettua tilaa eivätkä ole SMP-turvallisia atomisia laskureita.
- IRQ1 pysyy peitettynä ja PS/2-näppäimistö sekä hiiri luetaan kyselyllä.
- I/O APIC -alustus tukee vain nykyistä tarkistettua MMIO-osoitetta ja yhden ohjaimen tapausta.
- APICin valmius on tallennettu alustustulos, ei takuu kaikkien APIC-ominaisuuksien toiminnasta.

## Harjoitukset

1. Jos IRQ1 on peitetty ja sen laskuri pysyy nollassa, mitä voit päätellä ja mitä et?
2. Miksi `irq`-komennon pitää näyttää sekä I/O APICin valmius että peitettyjen merkintöjen määrä?
3. Selitä, miksi ajastimen tikit eivät ole kalibroitu seinäkello.
4. Suunnittele testi funktiolle `pic_irq_is_masked` muuttamatta laitteiston maskia.
5. Mitä synkronointia tarvitaan, ennen kuin jaettuja IRQ-laskureita voi pitää luotettavina usean CPU-ytimen välillä?

## Hyväksymiskriteerit

- [ ] PIC-maskin tarkistus on vain luku -toiminto ja hylkää IRQ-numerot alueen 0–15 ulkopuolelta.
- [ ] Komento näyttää ohjaimen tilan erillään keskeytysten toimituslaskureista.
- [ ] CI tarkistaa uusien diagnostiikkatietojen tunnisteet ja desimaalimuodon.
- [ ] Dokumentaatio ei rinnasta alustettua ohjainta toimivaan laitteeseen.
- [ ] Laitekeskeytystä ei oteta käyttöön ilman oikeaa käsittelijää ja omistajuusmallia.

---

[← Edellinen oppitunti](./37-decimal-digit-self-test.md) · [Kurssin sisällysluettelo](./README.md)
