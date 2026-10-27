# Alkuvaiheen sivutaulut

Copyright (c) 2026 ItsDropper. MIT-lisenssi; katso projektin juuren LICENSE.

## Tavoite ja selitys

Sivutus muuntaa virtuaaliosoitteita fyysisiksi osoitteiksi ja voi määrittää muistialueiden käyttöoikeuksia. VNTOSin alkuvaiheen identiteettikartoituksessa tietyllä alueella virtuaaliosoite ja fyysinen osoite ovat samat.

## Sanasto

Virtuaaliosoite on koodin käyttämä osoite. Fyysinen osoite viittaa RAM-muistiin. Sivutaulu kuvaa osoitteiden muunnoksia. Identiteettikartoitus tarkoittaa, että osoite A kartoitetaan fyysiseen osoitteeseen A.

## Käytännön harjoitus

Tutki boot.S-tiedoston sivutauluja. Selvitä niiden sijainti, kattama muistialue ja kohta, jossa ylimmän taulun osoite asetetaan CR3-rekisteriin. Piirrä osoitteen muunnos nuolilla.

## Yleinen virhe ja turvallisuus

Identiteettikartoitus ei ole täydellinen virtuaalimuistijärjestelmä eikä takaa prosessieristystä. Myös sivujen rajat ja käyttöoikeudet ovat tärkeitä.

## Tarkista ymmärtäminen

Mitä hyötyä virtuaaliosoitteista on? Miksi identiteettikartoitus auttaa käynnistyksessä?

## Ennen kuin jatkat

Varmista, että osaat selittää tämän oppitunnin pääkäsitteen omin sanoin, nimetä lähdekoodin kohdan, jossa se näkyy, ja kertoa, millä havainnolla testaisit toimintaa. Jos et osaa, palaa tiedostoon ja seuraa koodia vielä kerran.


## Syvempi harjoitus: seuraa koodia vaiheittain

Kun seuraat osoitetta, merkitse erikseen virtuaaliosoite, sivutaulun taso ja fyysinen osoite. Näin huomaat, ettei kaikkia osoitteita voi käsitellä samana asiana. Tarkista myös kartoituksen rajat.

Tee harjoitus näin:
1. **Ennusta:** kirjoita yhdellä lauseella, mitä uskot tämän osan tekevän.
2. **Etsi lähtökohta:** etsi keskeinen funktio tai käsky ja selvitä, kuka sitä kutsuu.
3. **Seuraa tietoa:** kirjaa syötteet, muutettu tila ja seuraava vaihe.
4. **Etsi virhepolku:** mitä tapahtuu, jos tieto puuttuu, laite ei vastaa tai oletettu tila ei ole voimassa?
5. **Todenna:** määritä havainto, joka osoittaisi toiminnon tapahtuneen. Se, ettei kernel kaadu heti, ei ole riittävä testi.
6. **Vertaa:** tarkista, vastasiko lähdekoodi alkuperäistä ennustustasi.

Kirjoita lopuksi omaan sanastoosi uusi käsite, sen selitys ja esimerkki tästä tiedostosta. Kernelin ymmärtäminen tarkoittaa osan vastuun, rajapinnan, riippuvuuksien ja rajoitusten tuntemista — ei pelkkää koodin silmäilyä.

---


## Koodiesimerkki: tarkista sivutaulun present-bitti

```vnt
fun virt_translate(address: i64) -> i64 {
    if address < 0 || address >= 1073741824 {
        return -1
    }
    let directory: i64 = virt_page_directory_base()
    let directory_index: int = (address / 2097152) % 512
    let pde: i64 = 0
    unsafe {
        pde = mem_read_u64(directory, directory_index * 8)
    }
    if pde % 2 == 0 {
        return -1
    }
    if (pde / 128) % 2 == 1 {
        let base: i64 = pde - (pde % 2097152)
        return base + (address % 2097152)
    }
    // Tavallisen 4 KiB PTE:n käsittely jatkuu tässä funktiossa.
```

Tämä on os/mm/virt.vnt:n todellinen funktio-ote. Osoite rajataan ensin bootstrapin ensimmäiseen 1 GiB:iin. Hakemiston indeksi valitaan 2 MiB:n sivun perusteella, PDE luetaan vasta tarkistusten jälkeen ja present-bitti tarkistetaan ennen käyttöä. Jos PDE kuvaa suurta sivua, fyysinen perusosoite yhdistetään 2 MiB:n sisäiseen offsetiin.

**Tee itse:** Seuraa koko virt_translate-funktiota tiedostossa. Selvitä, miten se käsittelee 4 KiB:n PTE:n, puuttuvan kartoituksen ja osoitteen sivun sisällä. Älä laajenna aluetta yli 1 GiB:n, ennen kuin bootstrapin sivutaulut tukevat sitä.


## Toteutuslaboratorio: Alkuvaiheen sivutaulut

Tässä harjoituksessa toteutat tai jäljität toimivan osan, et vain kuvaile käsitettä.

**Tutkittavat tiedostot:** `os/arch/x86_64/boot.S`, `os/mm/virt.vnt`

**Tavoite:** Jäljitä virtuaaliosoitteen käännös nelitasoisessa sivutaulussa. Tarkista present-bitit ja suuret sivut ennen dereferointia. Pidä nykyisen identiteettikartoituksen raja näkyvissä.

### Toteutus vaiheittain

1. **Rajaa vastuu:** Etsi PML4-, PDPT- ja page directory -taulujen määrittelyt.
   - Kirjaa lähdekoodin kohta, joka todistaa tämän.
   - Tarkista myös kutsujan odotukset ennen muutosta.

2. **Etsi nykyinen toteutus:** Laske yhdelle osoitteelle indeksit ja sivun offset.
   - Kirjaa lähdekoodin kohta, joka todistaa tämän.
   - Tarkista myös kutsujan odotukset ennen muutosta.

3. **Kirjaa sopimus:** Selitä, miksi nykyinen bootstrap käyttää 2 MiB:n sivuja ensimmäisen 1 GiB:n kartoitukseen.
   - Kirjaa lähdekoodin kohta, joka todistaa tämän.
   - Tarkista myös kutsujan odotukset ennen muutosta.

4. **Tee pienin muutos:** Etsi virtuaalimuistin lukufunktio ja sen rajat.
   - Kirjaa lähdekoodin kohta, joka todistaa tämän.
   - Tarkista myös kutsujan odotukset ennen muutosta.

5. **Tarkista riippuvuudet:** Älä lisää uutta taulutasoa ennen kuin kehysallokointi ja TLB-vaikutus ovat selvät.
   - Kirjaa lähdekoodin kohta, joka todistaa tämän.
   - Tarkista myös kutsujan odotukset ennen muutosta.

6. **Käännä:** Testaa tunnettu identiteettiosoite ja puuttuvan kartoituksen tapaus.
   - Kirjaa lähdekoodin kohta, joka todistaa tämän.
   - Tarkista myös kutsujan odotukset ennen muutosta.

### Sopimus ja rajat

- Määritä syötteet, paluuarvot ja virhetilanteet ennen toteutusta.
- Tarkista osoitteet, pituudet, indeksit ja tilan alustaminen ennen käyttöä.
- Erota CPU:n vaatima mekanismi VNT:n päätöksenteosta.
- Älä väitä nykyisen rajoitetun toteutuksen tukevan ominaisuuksia, joita koodi ei sisällä.
- Tarkista, ettei uusi toiminto vaadi alustamatonta moduulia tai linkittämätöntä symbolia.
- Pidä diagnostiikka rajattuna ja vältä odottamista keskeytyskontekstissa.

### Testaa jokainen polku

1. Kirjoita ennuste ennen käännöstä.
2. Suorita `bash os/build.sh` projektin juuressa.
3. Tarkista ensimmäinen epäonnistunut komento.
4. Käynnistä kernel QEMU:ssa ja tallenna sarjaportin tuloste.
5. Testaa normaali tapaus, rajatapaus ja virheellinen syöte.
6. Tarkista ettei virheellinen pyyntö muuta muistia tai laitteiston tilaa.
7. Jos testaus ei ole käytettävissä, merkitse tulos vahvistamattomaksi.

### Tavalliset virheet

- Osoitteen tai pituuden tarkistus tehdään vasta luku-/kirjoitusoperaation jälkeen.
- Uusi funktio ei ole mukana import-ketjussa.
- Paluuarvon merkitys ei ole sama kutsujalle ja toteuttajalle.
- Diagnostiikka peittää alkuperäisen vian.
- Käännöksen onnistumista pidetään toimintatestinä.

### Kirjaa toteutus

Dokumentoi muutetut tiedostot, funktiosopimus, build-tulos, QEMU-havainto, negatiiviset testit ja rajoitukset. Näin toinen kehittäjä pystyy toistamaan tarkistuksen.

---

[← Edellinen oppitunti](./09-long-mode.md) · [Sisällysluettelo](./README.md) · [Seuraava oppitunti →](./11-elf-and-linker.md)
