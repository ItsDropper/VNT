# x86-64 long mode

Copyright (c) 2026 ItsDropper. MIT-lisenssi; katso projektin juuren LICENSE.

## Tavoite ja selitys

64-bittiseksi käännetty kernel ei voi hypätä suoraan long modeen. Käynnistysvaiheen pitää asettaa CPU:n tilarekisterit, valmistella sivutaulut ja vaihtaa suoritusmuoto arkkitehtuurin vaatimalla tavalla.

## Sanasto

CR0, CR3 ja CR4 ovat CPU:n ohjausrekistereitä. EFER sisältää muun muassa long modeen liittyviä asetuksia. CPU-tila määrittää, miten käskyt ja osoitteet tulkitaan.

## Käytännön harjoitus

Etsi boot.S-tiedostosta kohta, jossa CR3 asetetaan ja CPU:n tila vaihtuu. Kirjoita, mitä tehdään ennen siirtymää ja mitä sen jälkeen. Tarkista käskyjen merkitys arkkitehtuuridokumentaatiosta ennen muutoksia.

## Yleinen virhe ja turvallisuus

Long mode ei tarkoita, että muistinhallinta olisi valmis tai että laiteajurit toimisivat. Se on CPU:n suoritusmuoto, ei kokonainen käyttöjärjestelmän ominaisuus.

## Tarkista ymmärtäminen

Miksi 64-bittinen käännös ei riitä? Miten long mode eroaa muistinhallinnasta?

## Ennen kuin jatkat

Varmista, että osaat selittää tämän oppitunnin pääkäsitteen omin sanoin, nimetä lähdekoodin kohdan, jossa se näkyy, ja kertoa, millä havainnolla testaisit toimintaa. Jos et osaa, palaa tiedostoon ja seuraa koodia vielä kerran.


## Syvempi harjoitus: seuraa koodia vaiheittain

Arkkitehtuurin rekistereitä ei pidä muuttaa arvauksella. Tarkista jokaisen bitin tarkoitus ja vaadittu järjestys luotettavasta x86-64-dokumentaatiosta.

Tee harjoitus näin:
1. **Ennusta:** kirjoita yhdellä lauseella, mitä uskot tämän osan tekevän.
2. **Etsi lähtökohta:** etsi keskeinen funktio tai käsky ja selvitä, kuka sitä kutsuu.
3. **Seuraa tietoa:** kirjaa, mitä arvoja funktio saa, mitä tilaa se muuttaa ja mitä tapahtuu seuraavaksi.
4. **Etsi virhepolku:** mitä jos tieto puuttuu, laite ei vastaa tai oletettu tila ei olekaan voimassa?
5. **Todenna:** määritä näkyvä havainto, joka osoittaisi toiminnon tapahtuneen. Pelkkä se, ettei kernel kaadu heti, ei ole riittävä testi.
6. **Tarkista oletus:** vertaa lähdekoodia alkuperäiseen ennustukseesi ja korjaa muistiinpanoja.

Kirjoita lopuksi omaan sanastoosi uusi käsite, sen selitys ja yksi konkreettinen esimerkki tästä tiedostosta. Kernelin ymmärtäminen ei ole käskyjen ulkoa muistamista: tärkeintä on hahmottaa osan vastuu, rajapinta, riippuvuudet ja rajoitukset.

---


## Koodiesimerkki: long mode -siirtymän edellytykset

```asm
    movl %cr4, %eax
    orl $0x20, %eax
    movl %eax, %cr4
    movl $pml4_table, %eax
    movl %eax, %cr3
    movl $0xc0000080, %ecx
    rdmsr
    orl $0x100, %eax
    wrmsr
    lgdt gdt64_pointer
    movl %cr0, %eax
    orl $0x80000000, %eax
    movl %eax, %cr0
    ljmp $0x08, $long_mode_entry
```

Katkelma on nykyisestä boot.S-tiedostosta. CR4.PAE otetaan käyttöön, CR3 saa sivutaulujen osoitteen, EFER.LME asetetaan MSR:n kautta, GDT ladataan ja CR0.PG kytketään päälle ennen kaukohyppyä 64-bittiseen koodisegmenttiin. Nämä asetukset ovat riippuvaisia toisistaan; pelkkä `.code64`-direktiivi ei vaihda CPU:n tilaa.

**Tee itse:** Tarkista tiedostosta järjestys ja debug-merkinnät `1`, `2`, `3`, `4` sekä `L`. Jos käynnistys pysähtyy, etsi viimeinen merkki ja rajaa vika juuri sitä seuraavaan vaiheeseen.


## Toteutuslaboratorio: x86-64 long mode

Tämä on toteutusohje, ei vain aiheesta kertova yhteenveto. Avaa lähdetiedostot ennen kuin muutat niitä.

**Tutkittavat tiedostot:** `os/arch/x86_64/boot.S`, `os/linker.ld`

**Tavoite:** Tarkista long mode -siirtymän vaatimukset järjestyksessä. CPU:n tila, sivutus ja segmenttivalinta muodostavat yhden sopimuksen. Älä vaihda järjestystä kokeilemalla sokkona.

### Toteutus vaiheittain

1. **Rajaa vastuu:** Etsi CR4.PAE:n asetus, EFER.LME ja CR0.PG:n asetus.
   - Kirjaa havainto ja perustele se lähdekoodilla.
   - Älä jatka seuraavaan muutokseen, jos tämä vaihe on epäselvä.

2. **Etsi nykyinen toteutus:** Varmista CR3:n osoittavan kelvollisiin sivutauluihin.
   - Kirjaa havainto ja perustele se lähdekoodilla.
   - Älä jatka seuraavaan muutokseen, jos tämä vaihe on epäselvä.

3. **Kirjaa sopimus:** Tarkista GDT:n 64-bittinen koodisegmentti ja kaukohypyn kohde.
   - Kirjaa havainto ja perustele se lähdekoodilla.
   - Älä jatka seuraavaan muutokseen, jos tämä vaihe on epäselvä.

4. **Tee pienin muutos:** Varmista pino ennen kuin kutsut käännettyä funktiota.
   - Kirjaa havainto ja perustele se lähdekoodilla.
   - Älä jatka seuraavaan muutokseen, jos tämä vaihe on epäselvä.

5. **Tarkista riippuvuudet:** Tarkista kääntäjän ABI ja -mno-red-zone-asetus.
   - Kirjaa havainto ja perustele se lähdekoodilla.
   - Älä jatka seuraavaan muutokseen, jos tämä vaihe on epäselvä.

6. **Käännä:** Jos boot pysähtyy, lisää tunnisteet ennen ja jälkeen jokaisen tilanvaiheen.
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

---

[← Edellinen oppitunti](./08-reading-boot-assembly.md) · [Sisällysluettelo](./README.md) · [Seuraava oppitunti →](./10-initial-paging.md)
