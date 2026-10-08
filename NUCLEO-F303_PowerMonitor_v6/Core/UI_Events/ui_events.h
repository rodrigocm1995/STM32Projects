#ifndef UI_EVENTS_H
#define UI_EVENTS_H

#ifdef __cplusplus
extern "C" {
#endif

/* Inicializa y vincula todos los eventos táctiles y callbacks de la GUI */
void UI_Events_Init(void);

/* Actualiza el osciloscopio y las etiquetas de graph_screen */
void UI_Events_UpdateGraph(void);

#ifdef __cplusplus
}
#endif

#endif /* UI_EVENTS_H */