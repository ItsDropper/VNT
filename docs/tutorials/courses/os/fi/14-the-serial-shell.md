# Sarjaportin komentotulkki

Copyright (c) 2026 ItsDropper. MIT-lisenssi; katso projektin juuren LICENSE.

## Tavoite ja selitys

Shell lukee tekstin, tunnistaa komennon ja kutsuu oikeaa toimintoa. Tyypillinen polku on syötteen lukeminen, rivin käsittely, komennon vertailu, käsittelijän kutsu ja tulostus.

## Sanasto

Jäsentäminen muuttaa tekstin ohjelman käsittelemäksi rakenteeksi. Komentokäsittelijä toteuttaa yhden komennon. Virheilmoitus kertoo, miksi syötettä ei voitu käsitellä.

## Käytännön harjoitus

Avaa os/kernel/shell.vnt. Etsi syötepuskuri, komennon tunnistus ja tuntemattoman komennon käsittely. Seuraa yhden komennon polkua alusta tulosteeseen ennen kuin lisäät uuden komennon.

## Yleinen virhe ja turvallisuus

Tarkista tyhjä syöte, liian pitkä rivi ja tuntematon komento. Älä anna käyttäjän tekstin toimia muistiosoitteena tai rajattoman puskurin sisältönä.

## Tarkista ymmärtäminen

Piirrä polku näppäimistöstä komentokäsittelijään. Missä kohtaa virheellinen syöte pitäisi hylätä?

## Ennen kuin jatkat

Varmista, että osaat selittää tämän oppitunnin pääkäsitteen omin sanoin, nimetä lähdekoodin kohdan, jossa se näkyy, ja kertoa, millä havainnolla testaisit toimintaa. Jos et osaa, palaa tiedostoon ja seuraa koodia vielä kerran.


## Syvempi harjoitus: seuraa koodia vaiheittain

Seuraa samaa syötemuuttujaa koko komentopolun läpi. Näin huomaat, missä välilyönnit, rivin loppu ja tuntematon nimi käsitellään. Tarkista myös, ettei uusi komento riko vanhoja komentoja.

Tee harjoitus näin:
1. **Ennusta:** kirjoita yhdellä lauseella, mitä uskot tämän osan tekevän.
2. **Etsi lähtökohta:** etsi keskeinen funktio tai käsky ja selvitä, kuka sitä kutsuu.
3. **Seuraa tietoa:** kirjaa syötteet, muutettu tila ja seuraava vaihe.
4. **Etsi virhepolku:** mitä tapahtuu, jos tieto puuttuu, laite ei vastaa tai oletettu tila ei ole voimassa?
5. **Todenna:** määritä havainto, joka osoittaisi toiminnon tapahtuneen. Se, ettei kernel kaadu heti, ei ole riittävä testi.
6. **Vertaa:** tarkista, vastasiko lähdekoodi alkuperäistä ennustustasi.

Kirjoita lopuksi omaan sanastoosi uusi käsite, sen selitys ja esimerkki tästä tiedostosta. Kernelin ymmärtäminen tarkoittaa osan vastuun, rajapinnan, riippuvuuksien ja rajoitusten tuntemista — ei pelkkää koodin silmäilyä.

---

[← Sisällysluettelo](./README.md) · [← Edellinen: Sarjaportti debuggausvälineenä](13-serial-io.md) · [Seuraava oppitunti: Selvitä ongelmat järjestelmällisesti →](15-debugging.md)
