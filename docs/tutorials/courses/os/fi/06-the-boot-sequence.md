# Mitä käynnistyksessä tapahtuu?

Copyright (c) 2026 ItsDropper. MIT-lisenssi; katso projektin juuren LICENSE.

## Tavoite ja selitys

Firmware käynnistää tietokoneen, käynnistyslatain etsii kernelin ja lataa sen muistiin, ja käynnistyskoodi valmistelee CPU:n ennen VNT-koodia. Jokainen vaihe olettaa, että edellinen on jättänyt CPU:n ja muistin oikeaan tilaan.

## Sanasto

Entry point on suorituksen aloituskohta. Pino säilyttää muun muassa funktiokutsujen paluuosoitteita. Firmware on emolevyn käynnistyskoodi. GRUB on käynnistyslatain.

## Käytännön harjoitus

Avaa os/arch/x86_64/boot.S. Etsi Multiboot2-otsake, pinon asetus, sivutaulujen valmistelu ja siirtyminen 64-bittiseen tilaan. Seuraa tiedostoa järjestyksessä ja piirrä vaiheet nuolilla.

## Yleinen virhe ja turvallisuus

Jos virhe tapahtuu ennen konsolin alustusta, sarjaportissa ei välttämättä näy mitään. Älä päättele hiljaisuudesta, ettei CPU olisi suorittanut käskyjä.

## Tarkista ymmärtäminen

Kuvaile ketju firmwaresta VNT-funktioon. Miksi kernel tarvitsee alkuvaiheen assemblya?

## Ennen kuin jatkat

Varmista, että osaat selittää tämän oppitunnin pääkäsitteen omin sanoin, nimetä lähdekoodin kohdan, jossa se näkyy, ja kertoa, millä havainnolla testaisit toimintaa. Jos et osaa, palaa tiedostoon ja seuraa koodia vielä kerran.


## Syvempi harjoitus: seuraa koodia vaiheittain

Käynnistys on sarja järjestettyjä siirtymiä. Jokaisessa vaiheessa on oletuksia rekistereistä, muistista ja pinosta. Jos oletus rikkoutuu, seuraava vaihe voi kaatua ennen virheilmoitusta.

Tee harjoitus näin:
1. **Ennusta:** kirjoita yhdellä lauseella, mitä uskot tämän osan tekevän.
2. **Etsi lähtökohta:** etsi keskeinen funktio tai käsky ja selvitä, kuka sitä kutsuu.
3. **Seuraa tietoa:** kirjaa, mitä arvoja funktio saa, mitä tilaa se muuttaa ja mitä tapahtuu seuraavaksi.
4. **Etsi virhepolku:** mitä jos tieto puuttuu, laite ei vastaa tai oletettu tila ei olekaan voimassa?
5. **Todenna:** määritä näkyvä havainto, joka osoittaisi toiminnon tapahtuneen. Pelkkä se, ettei kernel kaadu heti, ei ole riittävä testi.
6. **Tarkista oletus:** vertaa lähdekoodia alkuperäiseen ennustukseesi ja korjaa muistiinpanoja.

Kirjoita lopuksi omaan sanastoosi uusi käsite, sen selitys ja yksi konkreettinen esimerkki tästä tiedostosta. Kernelin ymmärtäminen ei ole käskyjen ulkoa muistamista: tärkeintä on hahmottaa osan vastuu, rajapinta, riippuvuudet ja rajoitukset.

---

[← Sisällysluettelo](./README.md) · [← Edellinen: Miten lähdekoodista syntyy kernel](05-build-pipeline.md) · [Seuraava oppitunti: GRUB ja Multiboot2 →](07-grub-and-multiboot2.md)

## Toteutuslaboratorio: Käynnistyksen vaiheet

Tämä on toteutusohje, ei vain aiheesta kertova yhteenveto. Avaa lähdetiedostot ennen kuin muutat niitä.

**Tutkittavat tiedostot:** `os/arch/x86_64/boot.S`, `os/kernel.vnt`, `os/linker.ld`

**Tavoite:** Tee käynnistyksestä jäljitettävä vaihe vaiheelta. Jokaisella vaiheella pitää olla sisäänmeno, ulostulo ja epäonnistumisen tunniste. Älä lisää ominaisuuksia ennen kuin peruspolku on selvä.

### Toteutus vaiheittain

1. **Rajaa vastuu:** Etsi entry-symboli ja seuraa hyppyä ensimmäiseen VNT-funktioon.
   - Kirjaa havainto ja perustele se lähdekoodilla.
   - Älä jatka seuraavaan muutokseen, jos tämä vaihe on epäselvä.

2. **Etsi nykyinen toteutus:** Merkitse milloin pino, sivutaulut ja 64-bittinen tila ovat valmiina.
   - Kirjaa havainto ja perustele se lähdekoodilla.
   - Älä jatka seuraavaan muutokseen, jos tämä vaihe on epäselvä.

3. **Kirjaa sopimus:** Lisää diagnostiset merkit kahden riskialttiin vaiheen ympärille.
   - Kirjaa havainto ja perustele se lähdekoodilla.
   - Älä jatka seuraavaan muutokseen, jos tämä vaihe on epäselvä.

4. **Tee pienin muutos:** Varmista, ettei myöhempi funktio käytä alustamatonta alijärjestelmää.
   - Kirjaa havainto ja perustele se lähdekoodilla.
   - Älä jatka seuraavaan muutokseen, jos tämä vaihe on epäselvä.

5. **Tarkista riippuvuudet:** Testaa sarjaportista viimeinen saavutettu merkki.
   - Kirjaa havainto ja perustele se lähdekoodilla.
   - Älä jatka seuraavaan muutokseen, jos tämä vaihe on epäselvä.

6. **Käännä:** Älä oleta, että ruudun tyhjyys kertoo missä vaiheessa vika on.
   - Kirjaa havainto ja perustele se lähdekoodilla.
   - Älä jatka seuraavaan muutokseen, jos tämä vaihe on epäselvä.

### Rajapinnan tarkistus

- Mikä funktio tai symboli aloittaa toiminnon?
- Kuka kutsuu sitä ja mitä arvoja kutsuja olettaa?
- Mitkä rekisterit, osoitteet tai muistialueet ovat käytössä?
- Mikä tarkistus tehdään ennen muistilukua tai laitekirjoitusta?
- Mitä tapahtuu, jos tarvittava tieto puuttuu tai on virheellinen?
- Mitkä osat ovat VNT-politiikkaa ja mitkä arkkitehtuurin pakottamaa assemblyä?
- Mitkä ominaisuudet ovat vielä toteuttamatta?

### Hyväksymistesti

1. Kirjoita odotettu tulos ennen käännöstä.
2. Suorita `bash os/build.sh` projektin juuressa.
3. Tarkista ensimmäinen epäonnistunut komento, älä vain viimeistä virheilmoitusta.
4. Käynnistä kernel QEMU:ssa ja kerää sarjaportin tuloste.
5. Testaa normaali tapaus, raja ja virheellinen syöte.
6. Älä ilmoita testiä läpäistyksi, jos et suorittanut sitä.

### Virheiden välttäminen

- Käännös ei todista, että GRUB tunnistaa ELF:n.
- ELF:n syntyminen ei todista, että CPU saavuttaa VNT-koodin.
- Älä tee useita toisistaan riippuvia muutoksia ennen ensimmäistä testiä.
- Älä poista diagnostisia merkkejä ennen kuin tiedät, mikä vaihe oli vikana.
- Älä kovakoodaa osoitetta, jos sen pitäisi tulla linker-symbolista tai bootloaderin tietorakenteesta.

### Muutosraportti

Kirjaa muutetut tiedostot, toteutettu sopimus, build-tulos, QEMU-havainto, virhepolut ja tunnetut rajoitukset. Jos testaus jäi tekemättä, merkitse se avoimeksi työksi.
