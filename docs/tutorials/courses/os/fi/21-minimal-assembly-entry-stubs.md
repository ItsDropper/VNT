# Pidä assembly-rajapinta pienenä

Copyright (c) 2026 ItsDropper. MIT-lisenssi; katso projektin juuren LICENSE.

## Tavoite ja selitys

Assembly on tarpeen arkkitehtuurin rajakohdissa, mutta siitä ei pidä tehdä koko kernelin kieltä. Keskeytyksen sisääntulo voi tallentaa rekisterit ja valmistella tilan ennen VNT-funktiokutsua. Tapahtuman varsinainen käsittely kuuluu yleensä VNT-koodiin.

## Sanasto

Stub on pieni välityskoodi. Calling convention määrittää argumenttien, rekisterien ja paluuarvojen käytön. Clobber tarkoittaa rekisteriä tai tilaa, jonka koodi voi muuttaa.

## Käytännön harjoitus

Lue os/arch/x86_64/interrupts.S. Kirjaa, mitä tehdään ennen VNT-käsittelijää ja sen jälkeen. Selvitä pinon muutokset ja tallennetut rekisterit. Vertaa VNT-funktion odottamaan tietorakenteeseen.

## Yleinen virhe ja turvallisuus

Inline-assembly ilman kääntäjälle ilmoitettuja muistivaikutuksia voi johtaa virheelliseen koodiin. Käytä määriteltyä ja testattua kääntäjärajapintaa, älä irrallista assembly-tekstiä synkronoinnin korvikkeena.

## Tarkista ymmärtäminen

Nimeä yksi tehtävä, joka vaatii assemblya, ja yksi, joka kuuluu VNT-koodiin.

## Ennen kuin jatkat

Varmista, että osaat selittää tämän oppitunnin pääkäsitteen omin sanoin, nimetä lähdekoodin kohdan, jossa se näkyy, ja kertoa, millä havainnolla testaisit toimintaa. Jos et osaa, palaa tiedostoon ja seuraa koodia vielä kerran.


## Syvempi harjoitus: seuraa koodia vaiheittain

Kun arvioit, kuuluuko koodi assemblyyn, kysy vaatiiko se tietyn käskyn, rekisterin tai pinon. Jos kyse on yleisestä päätöksenteosta, VNT-koodi on yleensä helpompi ylläpitää.

Tee harjoitus näin:
1. **Ennusta:** kirjoita, mitä uskot tämän osan tekevän.
2. **Etsi lähtökohta:** löydä keskeinen funktio tai käsky ja selvitä, kuka sitä kutsuu.
3. **Seuraa tietoa:** kirjaa syötteet, muutettu tila ja seuraava vaihe.
4. **Etsi virhepolku:** mitä tapahtuu, jos tieto puuttuu tai oletettu tila ei ole voimassa?
5. **Todenna:** määritä havainto, joka osoittaa toiminnon tapahtuneen. Se, ettei kernel kaadu heti, ei ole riittävä testi.
6. **Vertaa:** tarkista, vastasiko lähdekoodi alkuperäistä ennustustasi.

Kirjoita lopuksi omaan sanastoosi uusi käsite, sen selitys ja esimerkki tästä tiedostosta. Kernelin ymmärtäminen tarkoittaa vastuun, rajapinnan, riippuvuuksien ja rajoitusten tuntemista — ei pelkkää koodin silmäilyä.

---

[← Sisällysluettelo](./README.md) · [← Edellinen: Keskeytykset, poikkeukset ja IDT](20-interrupts-and-the-idt.md) · [Seuraava oppitunti: Käsittele keskeytyksiä VNT-kielellä →](22-vnt-interrupt-dispatch.md)
