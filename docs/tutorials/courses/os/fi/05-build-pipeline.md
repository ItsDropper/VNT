# Miten lähdekoodista syntyy kernel

Copyright (c) 2026 ItsDropper. MIT-lisenssi; katso projektin juuren LICENSE.

## Tavoite ja selitys

Kääntäjä käsittelee VNT-koodia, assembler kääntää assemblyn objektitiedostoksi, linkkeri yhdistää käännetyt osat ja paketointivaihe luo käynnistettävän ISO-kuvan. Jokainen vaihe voi epäonnistua erikseen.

## Sanasto

Objektitiedosto sisältää käännettyä koodia. Linkkeri ratkaisee symboliviittauksia ja sijoittaa osiot. ISO on levytiedosto, jonka QEMU voi esittää käynnistysmediana.

## Käytännön harjoitus

Avaa os/build.sh. Tee taulukko: komento, syöte, tulos. Etsi kohta, jossa kernel linkitetään, ja kohta, jossa ISO kootaan. Aja build vasta riippuvuuksien valmistuttua.

## Yleinen virhe ja turvallisuus

ISO-tiedoston olemassaolo ei todista, että GRUB tunnistaa kernelin tai että CPU pääsee aloituskohtaan. Linkitysvirhe voi olla seurausta aiemmasta käännösvirheestä.

## Tarkista ymmärtäminen

Mikä ero on kääntäjällä ja linkkerillä? Miksi buildin jokainen vaihe pitää ymmärtää erikseen?

## Ennen kuin jatkat

Varmista, että osaat selittää tämän oppitunnin pääkäsitteen omin sanoin, nimetä lähdekoodin kohdan, jossa se näkyy, ja kertoa, millä havainnolla testaisit toimintaa. Jos et osaa, palaa tiedostoon ja seuraa koodia vielä kerran.


## Syvempi harjoitus: seuraa koodia vaiheittain

Build-ketju on riippuvuusketju: jos varhainen vaihe epäonnistuu, myöhemmät virheet voivat olla seurausta siitä. Tallenna onnistuneen buildin tulos ja vertaa ensimmäistä poikkeavaa vaihetta.

Tee harjoitus näin:
1. **Ennusta:** kirjoita yhdellä lauseella, mitä uskot tämän osan tekevän.
2. **Etsi lähtökohta:** etsi keskeinen funktio tai käsky ja selvitä, kuka sitä kutsuu.
3. **Seuraa tietoa:** kirjaa, mitä arvoja funktio saa, mitä tilaa se muuttaa ja mitä tapahtuu seuraavaksi.
4. **Etsi virhepolku:** mitä jos tieto puuttuu, laite ei vastaa tai oletettu tila ei olekaan voimassa?
5. **Todenna:** määritä näkyvä havainto, joka osoittaisi toiminnon tapahtuneen. Pelkkä se, ettei kernel kaadu heti, ei ole riittävä testi.
6. **Tarkista oletus:** vertaa lähdekoodia alkuperäiseen ennustukseesi ja korjaa muistiinpanoja.

Kirjoita lopuksi omaan sanastoosi uusi käsite, sen selitys ja yksi konkreettinen esimerkki tästä tiedostosta. Kernelin ymmärtäminen ei ole käskyjen ulkoa muistamista: tärkeintä on hahmottaa osan vastuu, rajapinta, riippuvuudet ja rajoitukset.

---

[← Sisällysluettelo](./README.md) · [← Edellinen: VNT-kielen perusteet kernel-koodissa](04-vnt-for-kernels.md) · [Seuraava oppitunti: Mitä käynnistyksessä tapahtuu? →](06-the-boot-sequence.md)

## Koodiesimerkki: build-putken todelliset vaiheet

```bash
./build/vnt --emit-freestanding-asm os/kernel.vnt -o build/kernel.s
gcc -c -m64 -ffreestanding -fno-pic -fno-stack-protector -mno-red-zone build/kernel.s -o build/kernel.o
gcc -c -m64 -ffreestanding -fno-pic -fno-stack-protector -mno-red-zone os/arch/x86_64/boot.S -o build/boot.o
ld -nostdlib -static -z max-page-size=0x1000 -T os/linker.ld -o build/kernel.elf build/boot.o build/kernel.o build/interrupts.o build/ap_trampoline.o
```

Nämä komennot ovat os/build.sh-tiedostosta. VNT-kääntäjä emittoi ensin assemblyn. GCC:n assembler-vaihe tuottaa objektitiedostoja, ja ld yhdistää objektit linker-skriptin määrittelemäksi ELF-kerneliksi. Jokainen vaihe voi epäonnistua eri syystä.

**Tee itse:** Avaa os/build.sh ja seuraa nämä komennot samassa järjestyksessä. Kun virhe tulee, selvitä ensimmäinen epäonnistunut vaihe ja tarkista sen syötetiedosto ennen myöhempien virheilmoitusten tutkimista.


## Toteutuslaboratorio: Käännösputki

Tämä on toteutusohje, ei vain aiheesta kertova yhteenveto. Avaa lähdetiedostot ennen kuin muutat niitä.

**Tutkittavat tiedostot:** `os/build.sh`, `src/compiler/freestanding_x86.c`

**Tavoite:** Jäljitä lähdekoodista linkitettyyn ELF:ään. Älä sekoita kääntäjän, assemblerin ja linkkerin virheitä. Todista jokainen vaihe erikseen.

### Toteutus vaiheittain

1. **Rajaa vastuu:** Kirjoita ylös jokaisen build.sh-komennon syöte ja tuloste.
   - Kirjaa havainto ja perustele se lähdekoodilla.
   - Älä jatka seuraavaan muutokseen, jos tämä vaihe on epäselvä.

2. **Etsi nykyinen toteutus:** Tarkista, missä VNT-koodi muuttuu assemblyksi ja missä assembly objektiksi.
   - Kirjaa havainto ja perustele se lähdekoodilla.
   - Älä jatka seuraavaan muutokseen, jos tämä vaihe on epäselvä.

3. **Kirjaa sopimus:** Tarkista linkityksen syöteobjektit ja lopputuloksen polku.
   - Kirjaa havainto ja perustele se lähdekoodilla.
   - Älä jatka seuraavaan muutokseen, jos tämä vaihe on epäselvä.

4. **Tee pienin muutos:** Tee virhe, jonka voi havaita vain linkitysvaiheessa, erillisessä kokeessa äläkä riko main-haaraa.
   - Kirjaa havainto ja perustele se lähdekoodilla.
   - Älä jatka seuraavaan muutokseen, jos tämä vaihe on epäselvä.

5. **Tarkista riippuvuudet:** Tarkista että valinnaiset grub-file ja grub-mkrescue vaiheet eivät sekoitu ELF:n kääntöön.
   - Kirjaa havainto ja perustele se lähdekoodilla.
   - Älä jatka seuraavaan muutokseen, jos tämä vaihe on epäselvä.

6. **Käännä:** Kirjaa ensimmäinen epäonnistunut komento ja sen poistumiskoodi.
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
