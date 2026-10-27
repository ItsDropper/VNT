# Aloita tästä

Copyright (c) 2026 ItsDropper. MIT-lisenssi; katso projektin juuren LICENSE.

## Tavoite ja selitys

Kurssi opettaa lukemaan, rakentamaan, käynnistämään ja muuttamaan VNTOS-käyttöjärjestelmän ydintä. Aloitamme tietokoneen peruskäsitteistä ja etenemme käynnistykseen, konsoliin, keskeytyksiin, muistiin ja useaan CPU-ytimeen.

## Sanasto

Kernel tarkoittaa käyttöjärjestelmän ydintä. Lähdekoodi on ihmisen kirjoittamaa ohjelmatekstiä. Kääntäjä muuttaa lähdekoodin koneen suoritettavaksi koodiksi. QEMU jäljittelee tietokonetta ohjelmallisesti.

## Käytännön harjoitus

Avaa os/-hakemisto GitHubissa. Etsi os/kernel.vnt ja os/build.sh. Lue tiedostojen alut ja etsi, mistä kernelin toiminta näyttää alkavan. Älä vielä muuta koodia; harjoittele ensin koodin seuraamista.

## Yleinen virhe ja turvallisuus

Älä asenna keskeneräistä ydintä oikealle tietokoneelle tai kirjoita levykuvaa USB-tikulle. Käytä QEMUta. VNTOS on oppimisprojekti, ei valmis työpöytäkäyttöjärjestelmä.

## Tarkista ymmärtäminen

Mikä on kernel? Mitä kääntäjä tekee? Miksi käytämme QEMUta?

## Ennen kuin jatkat

Varmista, että osaat selittää tämän oppitunnin pääkäsitteen omin sanoin, nimetä lähdekoodin kohdan, jossa se näkyy, ja kertoa, millä havainnolla testaisit toimintaa. Jos et osaa, palaa tiedostoon ja seuraa koodia vielä kerran.


## Syvempi harjoitus: seuraa koodia vaiheittain

Tavoitteena ei ole opetella kaikkia tiedostonimiä ulkoa. Opettele kysymään: kuka kutsuu tätä, mitä tämä muuttaa, mitä oletetaan valmiiksi ja miten toiminnan voi todentaa.

Tee harjoitus näin:
1. **Ennusta:** kirjoita yhdellä lauseella, mitä uskot tämän osan tekevän.
2. **Etsi lähtökohta:** etsi keskeinen funktio tai käsky ja selvitä, kuka sitä kutsuu.
3. **Seuraa tietoa:** kirjaa, mitä arvoja funktio saa, mitä tilaa se muuttaa ja mitä tapahtuu seuraavaksi.
4. **Etsi virhepolku:** mitä jos tieto puuttuu, laite ei vastaa tai oletettu tila ei olekaan voimassa?
5. **Todenna:** määritä näkyvä havainto, joka osoittaisi toiminnon tapahtuneen. Pelkkä se, ettei kernel kaadu heti, ei ole riittävä testi.
6. **Tarkista oletus:** vertaa lähdekoodia alkuperäiseen ennustukseesi ja korjaa muistiinpanoja.

Kirjoita lopuksi omaan sanastoosi uusi käsite, sen selitys ja yksi konkreettinen esimerkki tästä tiedostosta. Kernelin ymmärtäminen ei ole käskyjen ulkoa muistamista: tärkeintä on hahmottaa osan vastuu, rajapinta, riippuvuudet ja rajoitukset.

---

[← Sisällysluettelo](./README.md) · [Seuraava oppitunti: Mitä käyttöjärjestelmä tekee? →](01-what-is-an-os.md)
