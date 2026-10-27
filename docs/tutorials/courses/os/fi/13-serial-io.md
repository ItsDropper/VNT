# Sarjaportti debuggausvälineenä

Copyright (c) 2026 ItsDropper. MIT-lisenssi; katso projektin juuren LICENSE.

## Tavoite ja selitys

Sarjaportti antaa kernelille yksinkertaisen tekstikanavan. QEMU voi ohjata sen tulosteen terminaaliin, vaikka näytönohjain tai graafinen käyttöliittymä ei toimisi. Siksi sarjaportti on hyödyllinen ensimmäisten käynnistysvaiheiden tutkimiseen.

## Sanasto

UART on sarjaliikenteen laiteosa. Polling tarkistaa laitteen tilaa toistuvasti. Debuggaus tarkoittaa virheen syyn selvittämistä havaintojen avulla.

## Käytännön harjoitus

Etsi os/-hakemistosta sarjaportin alustus- ja kirjoitusfunktiot. Seuraa, missä ensimmäinen viesti tulostetaan. Käynnistä kernel projektin ohjeiden mukaisesti QEMUssa ja vertaa tulostetta lähdekoodin polkuun.

## Yleinen virhe ja turvallisuus

Viimeinen tuloste rajaa ongelmaa mutta ei yksin osoita tarkkaa vikakohtaa. Liiallinen debug-tulostus voi muuttaa ajoitusta, erityisesti keskeytyskoodissa.

## Tarkista ymmärtäminen

Miksi sarjaportti auttaa ennen graafista käyttöliittymää? Mitä viimeinen viesti todistaa ja mitä ei?

## Ennen kuin jatkat

Varmista, että osaat selittää tämän oppitunnin pääkäsitteen omin sanoin, nimetä lähdekoodin kohdan, jossa se näkyy, ja kertoa, millä havainnolla testaisit toimintaa. Jos et osaa, palaa tiedostoon ja seuraa koodia vielä kerran.


## Syvempi harjoitus: seuraa koodia vaiheittain

Sarjaportin tuloste on havainto, ei täydellinen totuus. Tulosteen puuttuminen voi tarkoittaa, ettei koodi päässyt kohtaan, konsolia ei alustettu tai laiteasetukset eivät täsmää. Testaa hypoteeseja erikseen.

Tee harjoitus näin:
1. **Ennusta:** kirjoita yhdellä lauseella, mitä uskot tämän osan tekevän.
2. **Etsi lähtökohta:** etsi keskeinen funktio tai käsky ja selvitä, kuka sitä kutsuu.
3. **Seuraa tietoa:** kirjaa syötteet, muutettu tila ja seuraava vaihe.
4. **Etsi virhepolku:** mitä tapahtuu, jos tieto puuttuu, laite ei vastaa tai oletettu tila ei ole voimassa?
5. **Todenna:** määritä havainto, joka osoittaisi toiminnon tapahtuneen. Se, ettei kernel kaadu heti, ei ole riittävä testi.
6. **Vertaa:** tarkista, vastasiko lähdekoodi alkuperäistä ennustustasi.

Kirjoita lopuksi omaan sanastoosi uusi käsite, sen selitys ja esimerkki tästä tiedostosta. Kernelin ymmärtäminen tarkoittaa osan vastuun, rajapinnan, riippuvuuksien ja rajoitusten tuntemista — ei pelkkää koodin silmäilyä.

---

[← Sisällysluettelo](./README.md) · [← Edellinen: Freestanding-ohjelma](12-freestanding-vnt.md) · [Seuraava oppitunti: Sarjaportin komentotulkki →](14-the-serial-shell.md)

## Koodiesimerkki: sarjaportin lähetys ja vastaanotto

```vnt
fun serial_putc(value: int) {
    unsafe {
        asm volatile("movl -8(%rbp), %eax; movw $0x3fd, %dx; 1: inb %dx, %al; testb $0x20, %al; jz 1b; movw $0x3f8, %dx; movl -8(%rbp), %eax; outb %al, %dx")
    }
    return 0
}

fun serial_try_getc() -> int {
    let result: int = -1
    unsafe {
        asm volatile("movw $0x3fd, %dx; inb %dx, %al; testb $1, %al; jz 1f; movw $0x3f8, %dx; inb %dx, %al; movzbl %al, %eax; movq %rax, -8(%rbp); jmp 2f; 1: movq $-1, -8(%rbp); 2:")
    }
    return result
}
```

Tämä on todellinen toteutus os/serial.vnt-tiedostosta. COM1:n 0x3FD on line-status-rekisteri: bitillä 0 tarkistetaan vastaanotettu data ja bitillä 5 lähetysvalmius. 0x3F8 on datarekisteri. `serial_try_getc` palauttaa -1, jos merkkiä ei ole vielä saatavilla. Nykyiset inline-assembly-rutiinit käyttävät kääntäjän nykyisiä pinon offsetteja, joten niitä ei pidä kopioida sokkona muihin funktioihin.

**Tee itse:** Lisää diagnostiikkaa kutsumalla serial_puts olemassa olevassa alustuskohdassa. Älä tee rajoittamatonta polling-silmukkaa vastaanotolle: tyhjä syöte pitää pystyä tunnistamaan ja ohjaus palauttamaan kutsujalle.


## Toteutuslaboratorio: Sarjaportti

Tässä harjoituksessa toteutat tai jäljität toimivan osan, et vain kuvaile käsitettä.

**Tutkittavat tiedostot:** `os/serial.vnt`, `os/kernel.vnt`

**Tavoite:** Toteuta diagnostiikkatuloste, joka toimii ennen VGA-konsolia. Polling pitää rajata, jotta rikkinäinen UART ei jumiuta bootia ikuisesti. Älä tulosta pitkää tekstiä keskeytyskäsittelijästä.

### Toteutus vaiheittain

1. **Rajaa vastuu:** Etsi COM1:n data-, interrupt-enable- ja line-status-portit.
   - Kirjaa lähdekoodin kohta, joka todistaa tämän.
   - Tarkista myös kutsujan odotukset ennen muutosta.

2. **Etsi nykyinen toteutus:** Selvitä, miksi serial_putc odottaa lähetysvalmiutta ennen datan kirjoitusta.
   - Kirjaa lähdekoodin kohta, joka todistaa tämän.
   - Tarkista myös kutsujan odotukset ennen muutosta.

3. **Kirjaa sopimus:** Tarkista serial_try_getc:n ei-data-paluuarvo ja miten kutsuja käsittelee sen.
   - Kirjaa lähdekoodin kohta, joka todistaa tämän.
   - Tarkista myös kutsujan odotukset ennen muutosta.

4. **Tee pienin muutos:** Lisää lyhyt yksilöivä tunniste oikeaan alustuskohtaan.
   - Kirjaa lähdekoodin kohta, joka todistaa tämän.
   - Tarkista myös kutsujan odotukset ennen muutosta.

5. **Tarkista riippuvuudet:** Testaa merkki, merkkijono, rivinvaihto ja vastaanoton tyhjä tapaus.
   - Kirjaa lähdekoodin kohta, joka todistaa tämän.
   - Tarkista myös kutsujan odotukset ennen muutosta.

6. **Käännä:** Käytä sarjaporttia viimeisenä onnistuneena boot-vaiheen merkkinä.
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
