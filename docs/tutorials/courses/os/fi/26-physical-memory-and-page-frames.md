# Fyysinen muisti ja sivukehykset

Copyright (c) 2026 ItsDropper. MIT-lisenssi; katso projektin juuren LICENSE.

## Tavoite ja selitys

Fyysisen muistin varaaja antaa kernelille RAM-alueita. Tavallinen tapa on jakaa muisti 4096 tavun sivukehyksiin ja pitää kirjaa vapaista kehyksistä. Varaaja ei saa antaa käyttöön jo varattua kehystä tai firmwarelle kuuluvaa muistia.

## Sanasto

Sivukehys on fyysinen muistialue. Bittikartta tallentaa kunkin kehyksen tilan biteillä. Varaus merkitsee kehyksen käytetyksi, vapautus tekee sen taas saatavaksi. Invariantti on sääntö, jonka on aina pysyttävä totena.

## Käytännön harjoitus

Avaa os/mm/phys.vnt. Etsi alustustoiminto, varaus, vapautus, bittikartta ja vapaiden kehysten laskuri. Selvitä, miten käytettävät muistialueet valitaan ja mitä tapahtuu, kun vapaa muisti loppuu.

## Yleinen virhe ja turvallisuus

Varaaja ei ole yleisesti turvallinen usean CPU:n samanaikaisessa käytössä. Kaksi CPU:ta voi lukea saman vapaan bitin ennen päivitystä. Tarvitaan synkronointi ja moniydintestit.

## Tarkista ymmärtäminen

Miksi samaa kehystä ei saa antaa kahdelle kutsujalle? Miksi bittikartan ja laskurin muutokset on suojattava yhdessä?

## Ennen kuin jatkat

Varmista, että osaat selittää tämän oppitunnin pääkäsitteen omin sanoin, nimetä lähdekoodin kohdan, jossa se näkyy, ja kertoa, millä havainnolla testaisit toimintaa. Jos et osaa, palaa tiedostoon ja seuraa koodia vielä kerran.


## Syvempi harjoitus: seuraa koodia vaiheittain

Varaajan invariantteihin kuuluu, ettei samaa kehystä anneta kahdesti ja ettei vapaan kehyksen laskuri mene vääräksi. Kirjoita invariantti ennen ja jälkeen operaation, niin puuttuvat päivitykset löytyvät helpommin.

Tee harjoitus näin:
1. **Ennusta:** kirjoita, mitä uskot tämän osan tekevän.
2. **Etsi lähtökohta:** löydä keskeinen funktio tai käsky ja selvitä, kuka sitä kutsuu.
3. **Seuraa tietoa:** kirjaa syötteet, muutettu tila ja seuraava vaihe.
4. **Etsi virhepolku:** mitä tapahtuu, jos tieto puuttuu tai oletettu tila ei ole voimassa?
5. **Todenna:** määritä havainto, joka osoittaa toiminnon tapahtuneen. Se, ettei kernel kaadu heti, ei ole riittävä testi.
6. **Vertaa:** tarkista, vastasiko lähdekoodi alkuperäistä ennustustasi.

Kirjoita lopuksi omaan sanastoosi uusi käsite, sen selitys ja esimerkki tästä tiedostosta. Kernelin ymmärtäminen tarkoittaa vastuun, rajapinnan, riippuvuuksien ja rajoitusten tuntemista — ei pelkkää koodin silmäilyä.

---

[← Sisällysluettelo](./README.md) · [← Edellinen: Rakenna ja testaa keskeytyskernel](24-build-and-test-the-interrupt-kernel.md) · [Seuraava oppitunti: Loppuprojekti: tee todennettava parannus →](25-graduation-project.md)

## Koodiesimerkki: validoi Multiboot2-muistitiedon rajat

```vnt
fun phys_info_size() -> int {
    let info: i64 = phys_info_addr()
    if info == 0 {
        return 0
    }
    let size: int = 0
    unsafe {
        size = mem_read_u32(info, 0)
    }
    if size < 16 || info + size > 1073741824 {
        return 0
    }
    return size
}
```

Tämä on nykyisestä os/mm/phys.vnt-tiedostosta. Funktio tarkistaa, että Multiboot2-tietorakenteen osoite on hyväksytty ja koko vähintään otsakkeen minimikokoinen, eikä tietorakenne ylitä nykyistä 1 GiB:n identiteettikartoitusta. Vasta sen jälkeen kokoa voi käyttää tagien läpikäyntiin.

**Tee itse:** Jäljitä, miten phys_mmap_count kulkee tagista seuraavaan. Tarkista tag_size ennen kuin lasket seuraavan osoitteen. Älä lisää fyysistä muistia, joka ei ole kartoitettu tai joka kuuluu kernelille tai bootstrapille.


## Toteutuslaboratorio: Fyysinen muisti ja sivukehykset

Tässä osiossa rakennat tai validoit konkreettisen osan nykyisestä VNTOS-kernelistä.

**Tutkittavat tiedostot:** `os/mm/phys.vnt`, `os/arch/x86_64/boot.S`

**Tavoite:** Toteuta muistokartan lukeminen ja kehysvarauksen rajat turvallisesti. Multiboot2-data on ulkoista dataa ja tagien pituus vaihtelee. Nykyinen allokaattori on rajattu ensimmäiseen 1 GiB:n ikkunaan.

### Toteutus vaiheittain

1. **Rajaa vastuu:** Tarkista boot-magic, tietorakenteen koko ja osoiterajat ennen tagien lukua.
   - Perustele vaihe lähdekoodilla tai dokumentoidulla arkkitehtuurisopimuksella.
   - Kirjaa oletus, jonka tämä vaihe varmistaa.

2. **Etsi nykyinen toteutus:** Kulje tageissa tag_size-kentän mukaan ja hylkää virheellinen koko.
   - Perustele vaihe lähdekoodilla tai dokumentoidulla arkkitehtuurisopimuksella.
   - Kirjaa oletus, jonka tämä vaihe varmistaa.

3. **Kirjaa sopimus:** Erota käytettävät alueet kernelin ja bootstrapin varaamista alueista.
   - Perustele vaihe lähdekoodilla tai dokumentoidulla arkkitehtuurisopimuksella.
   - Kirjaa oletus, jonka tämä vaihe varmistaa.

4. **Tee pienin muutos:** Määritä bitmapin bitin merkitys ja sivukehyksen kohdistus.
   - Perustele vaihe lähdekoodilla tai dokumentoidulla arkkitehtuurisopimuksella.
   - Kirjaa oletus, jonka tämä vaihe varmistaa.

5. **Tarkista riippuvuudet:** Testaa tyhjä muistikartta, yksi vapaa kehys, loppunut muisti ja toistuva vapautus.
   - Perustele vaihe lähdekoodilla tai dokumentoidulla arkkitehtuurisopimuksella.
   - Kirjaa oletus, jonka tämä vaihe varmistaa.

6. **Käännä:** Älä palauta kehystä, jonka omistus tai rajat eivät ole tiedossa.
   - Perustele vaihe lähdekoodilla tai dokumentoidulla arkkitehtuurisopimuksella.
   - Kirjaa oletus, jonka tämä vaihe varmistaa.

### Turvallisuus- ja rajapintasopimus

- Validoi osoite, pituus, indeksi, tunniste ja laitetila ennen käyttöä.
- Älä lue muuttuvamittaista tietorakennetta ilman rajojen tarkistusta.
- Määritä selkeä virhepolku puuttuvalle laitteelle, virheelliselle datalle ja aikakatkaisulle.
- Erota “havaittu”, “alustettu”, “online” ja “käytössä” eri tiloiksi, jos ne tarkoittavat eri asioita.
- Älä oleta, että atominen muistikirjoitus yksin ratkaisee muistijärjestyksen tai omistajuuden.
- Dokumentoi rajoitukset, joita nykyinen kernel ei vielä ratkaise.

### Todennus

1. Kirjoita odotettu tila ja havainto ennen muutosta.
2. Suorita `bash os/build.sh` projektin juuressa.
3. Tarkista ensimmäinen epäonnistunut komento ja erottele build-ongelma laiteongelmasta.
4. Käynnistä vain QEMU:ssa ja kerää sarjaportin tuloste.
5. Testaa normaali tapaus, rajatapaus ja virheellinen tai puuttuva tieto.
6. Toista testi, jos toiminta riippuu ajoituksesta.
7. Älä väitä CI:n tai laitetestin läpäisseen, ellei siitä ole todellista tulosta.

### Tavalliset virheet

- Firmwaresta saatuun tietoon luotetaan ennen checksum- ja pituustarkistuksia.
- AP merkitään online-tilaan ennen kuin se saavuttaa sovitun valmiuspisteen.
- IPI lähetetään ennen kuin IDT ja vastaanottaja ovat valmiita.
- Jaettua muistia käytetään ilman omistajuus- tai lukitussopimusta.
- Laskurin kasvaminen tulkitaan liian laajasti todisteeksi ominaisuuden valmiudesta.

### Muutosraportti

Kirjaa muutetut tiedostot, rajapinta, hyväksymiskriteeri, build-tulos, QEMU-havainto, negatiiviset testit ja jäljelle jääneet rajoitukset.
