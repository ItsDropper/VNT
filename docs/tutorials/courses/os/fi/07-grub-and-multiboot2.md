# GRUB ja Multiboot2

Copyright (c) 2026 ItsDropper. MIT-lisenssi; katso projektin juuren LICENSE.

## Tavoite ja selitys

GRUB lataa kernelin ja välittää käynnistystietoja. Multiboot2 määrittää, miten yhteensopiva kernel tunnistetaan ja mitä tietoja käynnistyslataimen pitää välittää. Kernelin otsakkeen pitää noudattaa formaatin rakennetta.

## Sanasto

Otsake on tunnistettava tietorakenne. Magic-arvo on vakio, jonka avulla formaatti tunnistetaan. Boot information on käynnistyslataimen antamaa tietoa, esimerkiksi muistialueista.

## Käytännön harjoitus

Etsi Multiboot2-otsake tiedostosta os/arch/x86_64/boot.S. Katso sen kenttiä ja vertaa niitä os/build.sh-tiedoston GRUB- ja ISO-vaiheisiin. Älä muuta vakioita ilman, että tarkistat formaatin vaatimukset.

## Yleinen virhe ja turvallisuus

Jos GRUB ei löydä kerneliä, VNT-koodin debuggaus ei ole vielä oikea vaihe. Tarkista ensin, että ISO sisältää oikeat tiedostot.

## Tarkista ymmärtäminen

Mitä GRUB tekee? Miksi oikea kernel-tiedosto voi silti jäädä käynnistymättä?

## Ennen kuin jatkat

Varmista, että osaat selittää tämän oppitunnin pääkäsitteen omin sanoin, nimetä lähdekoodin kohdan, jossa se näkyy, ja kertoa, millä havainnolla testaisit toimintaa. Jos et osaa, palaa tiedostoon ja seuraa koodia vielä kerran.


## Syvempi harjoitus: seuraa koodia vaiheittain

Käynnistysformaatti on sopimus käynnistyslataimen ja kernelin välillä. Molempien pitää tulkita otsake samalla tavalla. Yksi väärä kenttä voi estää koko kernelin lataamisen.

Tee harjoitus näin:
1. **Ennusta:** kirjoita yhdellä lauseella, mitä uskot tämän osan tekevän.
2. **Etsi lähtökohta:** etsi keskeinen funktio tai käsky ja selvitä, kuka sitä kutsuu.
3. **Seuraa tietoa:** kirjaa, mitä arvoja funktio saa, mitä tilaa se muuttaa ja mitä tapahtuu seuraavaksi.
4. **Etsi virhepolku:** mitä jos tieto puuttuu, laite ei vastaa tai oletettu tila ei olekaan voimassa?
5. **Todenna:** määritä näkyvä havainto, joka osoittaisi toiminnon tapahtuneen. Pelkkä se, ettei kernel kaadu heti, ei ole riittävä testi.
6. **Tarkista oletus:** vertaa lähdekoodia alkuperäiseen ennustukseesi ja korjaa muistiinpanoja.

Kirjoita lopuksi omaan sanastoosi uusi käsite, sen selitys ja yksi konkreettinen esimerkki tästä tiedostosta. Kernelin ymmärtäminen ei ole käskyjen ulkoa muistamista: tärkeintä on hahmottaa osan vastuu, rajapinta, riippuvuudet ja rajoitukset.

---

[← Sisällysluettelo](./README.md) · [← Edellinen: Mitä käynnistyksessä tapahtuu?](06-the-boot-sequence.md) · [Seuraava oppitunti: Lue käynnistys-assemblya →](08-reading-boot-assembly.md)

## Koodiesimerkki: Multiboot2-otsake

```asm
 .section .multiboot,"a"
.balign 8
.global multiboot2_header
multiboot2_header:
    .long 0xe85250d6
    .long 0
    .long multiboot2_header_end - multiboot2_header
    .long -(0xe85250d6 + (multiboot2_header_end - multiboot2_header))
    .short 0
    .short 0
    .long 8
multiboot2_header_end:
```

Tämä on os/arch/x86_64/boot.S:n otsake. Magic, arkkitehtuurikenttä, kokonaispituus ja checksum muodostavat lataajan kanssa sovitun tiedostomuodon. `KEEP(*(.multiboot))` linker-skriptissä estää otsakkeen pudottamisen pois linkityksessä.

**Tee itse:** Älä muuta magic- tai pituuskenttiä arvaamalla. Tarkista käännöksen jälkeen `grub-file --is-x86-multiboot2 build/kernel.elf`, jos työkalu on asennettu, ja testaa ISO QEMU:ssa.


## Toteutuslaboratorio: GRUB ja Multiboot2

Tämä on toteutusohje, ei vain aiheesta kertova yhteenveto. Avaa lähdetiedostot ennen kuin muutat niitä.

**Tutkittavat tiedostot:** `os/arch/x86_64/boot.S`, `os/grub.cfg`, `os/linker.ld`

**Tavoite:** Varmista lataajan ja kernelin välinen sopimus. Otsakkeen, ELF:n ja entry-symbolin on sovittava yhteen. Käsittele bootloaderin tietoja ulkoisena syötteenä.

### Toteutus vaiheittain

1. **Rajaa vastuu:** Tarkista Multiboot2 magic, pituus, checksum ja otsakkeen kohdistus.
   - Kirjaa havainto ja perustele se lähdekoodilla.
   - Älä jatka seuraavaan muutokseen, jos tämä vaihe on epäselvä.

2. **Etsi nykyinen toteutus:** Varmista, että linker säilyttää .multiboot-osion ELF:n alussa.
   - Kirjaa havainto ja perustele se lähdekoodilla.
   - Älä jatka seuraavaan muutokseen, jos tämä vaihe on epäselvä.

3. **Kirjaa sopimus:** Tarkista bootloaderin välittämien rekisterien tallennus ennen niiden ylikirjoitusta.
   - Kirjaa havainto ja perustele se lähdekoodilla.
   - Älä jatka seuraavaan muutokseen, jos tämä vaihe on epäselvä.

4. **Tee pienin muutos:** Käytä grub-file-tarkistusta, jos työkalu on saatavilla.
   - Kirjaa havainto ja perustele se lähdekoodilla.
   - Älä jatka seuraavaan muutokseen, jos tämä vaihe on epäselvä.

5. **Tarkista riippuvuudet:** Käynnistä GRUB-ISO QEMU:ssa; älä testaa vain raakaa ELF:ää.
   - Kirjaa havainto ja perustele se lähdekoodilla.
   - Älä jatka seuraavaan muutokseen, jos tämä vaihe on epäselvä.

6. **Käännä:** Kun lataus epäonnistuu, tarkista ELF-segmentit ja otsake ennen VNT-koodia.
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
