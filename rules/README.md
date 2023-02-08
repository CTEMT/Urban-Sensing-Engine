```mermaid
graph BT;
    Infrastructure --> Thing
    BycicleLane --> Infrastructure
    Road --> Infrastructure
    NaturalAsset --> Thing
    GreenArea --> NaturalAsset
    UrbanPark --> GreenArea
    Service --> Thing
    PublicService --> Service
    PublicTransport --> PublicService
    Education --> PublicService
    WorshipPlace --> PublicService
    PublicSecurity --> PublicService
    PublicHealth --> PublicService
    CulturalHeritage --> Thing
    UrbanAsset --> CulturalHeritage
    HistoricBuilding --> UrbanAsset
```