# Selvitä ongelmat järjestelmällisesti

Copyright (c) 2026 ItsDropper. MIT-lisenssi; katso projektin juuren LICENSE.

## Tavoite ja selitys

Kernelin virhe voi olla käännösvirhe, linkitysvirhe, käynnistysjumi tai CPU-poikkeus. Aloita luokittelemalla ongelma: pääseekö build loppuun, käynnistyykö GRUB ja näkyykö sarjaportissa viestejä? Näin et tutki väärää kerrosta.

## Sanasto

Juurisyy on varsinainen aiheuttaja. Regressio on aiemmin toimineen asian rikkoutuminen. Poikkeus on CPU:n ilmoittama tapahtuma. Minimointi rajaa ongelman pienimpään toistettavaan tapaukseen.

## Käytännön harjoitus

Toista virhe, lue ensimmäinen varsinainen virhe, etsi viimeinen onnistunut vaihe ja tarkastele viimeksi muutettua koodia. Muuta yksi asia kerrallaan ja toista sama testi. Kirjaa tarkka komento ja tuloste.

## Yleinen virhe ja turvallisuus

Älä poista tarkistuksia vain saadaksesi buildin läpi. Varoitus voi kertoa todellisesta virheestä. Jos tulostetta ei näy, konsoli ei ehkä ole vielä alustettu.

## Tarkista ymmärtäminen

Mitä hyvään vikaraporttiin kuuluu? Miksi ensimmäinen virhe on usein viimeistä tärkeämpi?

## Ennen kuin jatkat

Varmista, että osaat selittää tämän oppitunnin pääkäsitteen omin sanoin, nimetä lähdekoodin kohdan, jossa se näkyy, ja kertoa, millä havainnolla testaisit toimintaa. Jos et osaa, palaa tiedostoon ja seuraa koodia vielä kerran.


## Syvempi harjoitus: seuraa koodia vaiheittain

Hyvä debuggaus muuttaa epävarmuuden testattaviksi vaihtoehdoiksi. Kerro viimeinen toimiva vaihe, tarkka komento ja muutos, jonka jälkeen virhe alkoi. Tämä tekee ongelmasta toistettavan.

Tee harjoitus näin:
1. **Ennusta:** kirjoita yhdellä lauseella, mitä uskot tämän osan tekevän.
2. **Etsi lähtökohta:** etsi keskeinen funktio tai käsky ja selvitä, kuka sitä kutsuu.
3. **Seuraa tietoa:** kirjaa syötteet, muutettu tila ja seuraava vaihe.
4. **Etsi virhepolku:** mitä tapahtuu, jos tieto puuttuu, laite ei vastaa tai oletettu tila ei ole voimassa?
5. **Todenna:** määritä havainto, joka osoittaisi toiminnon tapahtuneen. Se, ettei kernel kaadu heti, ei ole riittävä testi.
6. **Vertaa:** tarkista, vastasiko lähdekoodi alkuperäistä ennustustasi.

Kirjoita lopuksi omaan sanastoosi uusi käsite, sen selitys ja esimerkki tästä tiedostosta. Kernelin ymmärtäminen tarkoittaa osan vastuun, rajapinnan, riippuvuuksien ja rajoitusten tuntemista — ei pelkkää koodin silmäilyä.

---

[← Sisällysluettelo](./README.md) · [← Edellinen: Sarjaportin komentotulkki](14-the-serial-shell.md) · [Seuraava oppitunti: Testaa toimintaa, älä vain käännöstä →](16-testing.md)
