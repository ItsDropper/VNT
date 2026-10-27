# Lue käynnistys-assemblya

Copyright (c) 2026 ItsDropper. MIT-lisenssi; katso projektin juuren LICENSE.

## Tavoite ja selitys

Assembly näyttää CPU:n käskyt ja rekisterien käytön suoraan. Se on hyödyllinen, kun asetetaan ohjausrekistereitä tai käsitellään pinoa. Yhden käskyn merkitys riippuu rekisterien aiemmista arvoista ja CPU:n nykyisestä tilasta.

## Sanasto

Rekisteri on CPU:n sisäinen tallennuspaikka. Label nimeää koodikohdan. Hyppy muuttaa suorituksen seuraavaa osoitetta. Call ja ret liittyvät funktiokutsuun ja paluuseen.

## Käytännön harjoitus

Lue os/arch/x86_64/boot.S 5–10 riviä kerrallaan. Kirjoita jokaisesta rivistä, mitä rekisteriä tai muistia käytetään ja mihin suoritus jatkuu. Etsi käskyt mov, jmp, call ja ret ja selvitä niiden käyttö tässä yhteydessä.

## Yleinen virhe ja turvallisuus

Pidä assembly pienenä. Tavallinen päätöksenteko kannattaa tehdä VNT-kielessä, jos se on mahdollista turvallisesti.

## Tarkista ymmärtäminen

Valitse viisi käskyä ja selitä niiden vaikutus juuri tässä koodissa.

## Ennen kuin jatkat

Varmista, että osaat selittää tämän oppitunnin pääkäsitteen omin sanoin, nimetä lähdekoodin kohdan, jossa se näkyy, ja kertoa, millä havainnolla testaisit toimintaa. Jos et osaa, palaa tiedostoon ja seuraa koodia vielä kerran.


## Syvempi harjoitus: seuraa koodia vaiheittain

Älä lue assemblya vain käsky kerrallaan. Seuraa rekisterien arvoja ennen käskyä ja sen jälkeen. Kirjoita esimerkiksi rekisterin arvo ennen mov-käskyä ja sen jälkeen.

Tee harjoitus näin:
1. **Ennusta:** kirjoita yhdellä lauseella, mitä uskot tämän osan tekevän.
2. **Etsi lähtökohta:** etsi keskeinen funktio tai käsky ja selvitä, kuka sitä kutsuu.
3. **Seuraa tietoa:** kirjaa, mitä arvoja funktio saa, mitä tilaa se muuttaa ja mitä tapahtuu seuraavaksi.
4. **Etsi virhepolku:** mitä jos tieto puuttuu, laite ei vastaa tai oletettu tila ei olekaan voimassa?
5. **Todenna:** määritä näkyvä havainto, joka osoittaisi toiminnon tapahtuneen. Pelkkä se, ettei kernel kaadu heti, ei ole riittävä testi.
6. **Tarkista oletus:** vertaa lähdekoodia alkuperäiseen ennustukseesi ja korjaa muistiinpanoja.

Kirjoita lopuksi omaan sanastoosi uusi käsite, sen selitys ja yksi konkreettinen esimerkki tästä tiedostosta. Kernelin ymmärtäminen ei ole käskyjen ulkoa muistamista: tärkeintä on hahmottaa osan vastuu, rajapinta, riippuvuudet ja rajoitukset.

---


## Koodiesimerkki: säilytä bootloaderin rekisterit

```asm
boot32_entry:
    /* Preserve the Multiboot2 register contract before using EAX/EBX. */
    movl %eax, multiboot2_boot_magic
    movl %ebx, multiboot2_info_ptr
    movw $0xe9, %dx
    movb $66, %al
    outb %al, %dx
    cli
    cld
    movl $boot_stack_top, %esp
```

`EAX` sisältää Multiboot2-magic-arvon ja `EBX` osoittaa bootloaderin tietorakenteeseen. Ne tallennetaan ennen kuin rekistereitä käytetään muuhun. `cli` estää maskattavat keskeytykset bootstrapin aikana, `cld` asettaa merkkijonokäskyjen suunnan ja `ESP` saa bootstrap-pinon osoitteen.

**Tee itse:** Jäljitä jokainen käsky taulukkoon: rekisteri ennen käskyä, käskyn vaikutus ja rekisteri sen jälkeen. Älä muuta rekisterin käyttöä ennen kuin olet selvittänyt, käytetäänkö sen alkuperäistä arvoa myöhemmin.


## Toteutuslaboratorio: Käynnistysassemblyn lukeminen

Tämä on toteutusohje, ei vain aiheesta kertova yhteenveto. Avaa lähdetiedostot ennen kuin muutat niitä.

**Tutkittavat tiedostot:** `os/arch/x86_64/boot.S`

**Tavoite:** Selitä jokaisen käskyn vaikutus rekistereihin ja muistiin. Assemblymuutos voi vaikuttaa kaikkiin myöhempiin vaiheisiin. Tee ensin käskyjen jäljitys, sitten muutos.

### Toteutus vaiheittain

1. **Rajaa vastuu:** Valitse 10 peräkkäistä käskyä ja kirjaa rekisteri ennen ja jälkeen.
   - Kirjaa havainto ja perustele se lähdekoodilla.
   - Älä jatka seuraavaan muutokseen, jos tämä vaihe on epäselvä.

2. **Etsi nykyinen toteutus:** Etsi pinoon liittyvät käskyt ja laske muutetut tavut.
   - Kirjaa havainto ja perustele se lähdekoodilla.
   - Älä jatka seuraavaan muutokseen, jos tämä vaihe on epäselvä.

3. **Kirjaa sopimus:** Etsi ohjaussiirtymät ja kirjaa niiden kohde.
   - Kirjaa havainto ja perustele se lähdekoodilla.
   - Älä jatka seuraavaan muutokseen, jos tämä vaihe on epäselvä.

4. **Tee pienin muutos:** Tarkista, mitkä käskyt suoritetaan 32-bittisessä ja mitkä 64-bittisessä tilassa.
   - Kirjaa havainto ja perustele se lähdekoodilla.
   - Älä jatka seuraavaan muutokseen, jos tämä vaihe on epäselvä.

5. **Tarkista riippuvuudet:** Lisää vain yksi debug-porttimerkki ja varmista sen sijainti tulosteessa.
   - Kirjaa havainto ja perustele se lähdekoodilla.
   - Älä jatka seuraavaan muutokseen, jos tämä vaihe on epäselvä.

6. **Käännä:** Älä muuta rekisterin käyttöä ennen kuin olet löytänyt sen aiemman arvon omistajan.
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

[← Edellinen oppitunti](./07-grub-and-multiboot2.md) · [Sisällysluettelo](./README.md) · [Seuraava oppitunti →](./09-long-mode.md)
